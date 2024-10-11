const express = require("express");
const mongoose = require("mongoose");
const morgan = require("morgan");
const bodyParser = require("body-parser");
const mqtt = require("mqtt");
const cors = require("cors");
const moment = require("moment");
// Route
const LoraDataRoute = require("./routes/LoraData");
const ControlRoute = require("./routes/Control");
const ConditionRoute = require("./routes/Condition");
const events = require("./models/LoraData");
const eventsControl = require("./models/Control");
const eventsCondition = require("./models/Condition");
const { log } = require("console");
// const findCondition = require("./handleData/handleCondition");

const app = express();

app.use(morgan("dev"));
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());
app.use(cors());
app.options("*", cors());

const PORT = process.env.PORT || 5000;

const server = require("http").createServer(app);
const io = require("socket.io")(server, {
  cors: {
    origin: "*",
  },
});

//  MongoDB database
const db = mongoose.connection;
let dataArr = [];

db.on("error", (err) => {
  console.log("error connecting to MongoDB", err);
});
db.on("connected", async () => {
  console.log("MongoDB Connected!");
});

/**
 * Config MQTT
 */
// Config MQTT
const client = mqtt.connect("mqtt://mqtt.flespi.io:1883", {
  username: "ah2cbuQ57EsGfyGVtW7939tlbBprFq17Gwdbie3jcRiG57SHrZduYBRGhotdrrcL",
  password: "",
});
const topic = "/flespi/qos0";
const topic2 = "/flespi/qos1";

client.on("connect", async () => {
  await mongoose.connect(
    "mongodb+srv://hainam20:20032002nam%40@cluster0.du4h7t4.mongodb.net/LoraDB"
  ),
    { useNewUrlParser: true, useUnifiedTopology: true };
  console.log("MQTT Connected!!");
  client.subscribe(topic);
  client.subscribe("alert");
});

/**
 * Config MongoDB
 */
server.listen(PORT, async () => {
  // Connect MongoDb
  await mongoose.connect(
    "mongodb+srv://hainam20:20032002nam%40@cluster0.du4h7t4.mongodb.net/LoraDB"
  ),
    { useNewUrlParser: true, useUnifiedTopology: true };
  console.log(`app listening on port ${PORT}`);
});

/**
 * Connect Socket.io
 */
io.on("connection", (socket) => {
  console.log("client connected");
  // handle status Relay
  socket.on("status_Relay", async (status) => {
    console.log(status);

    await controlRelay(status);
    await saveControlData(status);
    await sendRelayData();
  });
});
const controlRelay = async (status) => {
  if (status.status === true) {
    client.publish(topic2, `ON_ID${status.ID}\0`);
  } else {
    client.publish(topic2, `OFF_ID${status.ID}\0`);
  }
  console.log(status);
};
// transmit data via MQTT into Flespi broker
client.on("message", async (topic, message) => {
  let count = 0;
  let data = message.toString();
  if (data.length > 30) {
    try {
      data = JSON.parse(data);
      await pushData(data);
      const condition = await findCondition(data.ID);
      await handleCondition(condition, data);
    } catch (error) {}
  } else {
    // saveControlData(data);
    try {
      data = JSON.parse(data);
      if (data.relay == 1) data.relay = true;
      else data.relay = false;
      let status = { ID: data.ID, status: data.relay };
      await io.emit("relay", status);
    } catch (error) {}
  }
  if (topic == "alert") {
    await io.emit("alert", data);
    console.log(data, topic);
  }
  // const payload = JSON.stringify(condition);
  // console.log(condition);
  // client.publish(topic2, payload);
});

const pushData = async (data) => {
  if (data && data.ID >= 0) {
    dataArr[data.ID] = data;
    console.log(dataArr);
    await io.emit("mqtt_data", dataArr);
  }
  try {
    await saveData(data);
  } catch (error) {
    console.log("...");
  }
};
// Websocket
saveData = async (data) => {
  data = new events(data);
  data = await data.save();
};
saveControlData = async (data) => {
  data = new eventsControl(data);
  data = await data.save();
};
const findCondition = async (ID) => {
  const data = await eventsCondition
    .find({ ID: ID })
    .limit(1)
    .sort({ createdAt: -1 })
    .select(
      "-_id -tempState._id -soilState._id -waterState._id -waterTimeState._id -createdAt -updatedAt -__v"
    )
    .catch((error) => {
      console.log(error);
    });
  return data;
};

const handleCondition = async (condition, data) => {
  try {
    const startDate = moment(
      condition[0].waterTimeState.startDate,
      "YYYY-MM-DD HH:mm"
    );
    const endDate = moment(
      condition[0].waterTimeState.endDate,
      "YYYY-MM-DD HH:mm"
    );
    const currentDate = moment();
    // console.log(data, condition);
    if (condition[0].mode === "Automatic") {
      if (condition[0].tempState.state === true) {
        if (data.adc_val <= condition[0].tempState.value) {
          const status = { ID: data.ID, status: true };
          await controlRelay(status);
        }
      }
      if (condition[0].soilState.state === true) {
        if (data.adc_val >= condition[0].soilState.value) {
          const status = { ID: data.ID, status: false };
          await controlRelay(status);
        }
      }
      if (condition[0].waterState.state === true) {
        if (data.temp >= condition[0].waterState.value) {
          const status = { ID: data.ID, status: true };
          await controlRelay(status);
        }
      }
      if (condition[0].waterTimeState.state === true) {
        if (startDate.isSame(currentDate, "minute")) {
          const status = { ID: data.ID, status: true };
          await controlRelay(status);
        } else if (endDate.isSame(currentDate, "minute")) {
          const status = { ID: data.ID, status: false };
          await controlRelay(status);
        }
      }
    }
  } catch (error) {}
};

sendRelayData = async () => {
  try {
    const relayData = await eventsControl
      .find()
      .sort({ createdAt: -1 })
      .limit(5)
      .exec();
    io.emit("relay_data", relayData);
  } catch (error) {
    console.error("Error fetching relay data:", error);
  }
};
app.use("/api/condition", ConditionRoute);
app.use("/api/loradata", LoraDataRoute);
app.use("/api/control", ControlRoute);
