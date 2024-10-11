const eventsCondition = require("../models/Condition");

const findCondition = async () => {
  const data = await eventsCondition
    .find({ ID: ID })
    .limit(1)
    .sort({ createdAt: -1 })
    .then((response) => {
      res.json({ response });
    })
    .catch((error) => {
      res.json({ message: "An occured Error" });
      console.log(error);
    });
  console.log(data);
  const { tempState, soilState, waterState } = data;
};
module.export = { findCondition };
