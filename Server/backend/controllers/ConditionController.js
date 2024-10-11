const { response } = require("express");
const { error, log } = require("console");
const Condition = require("../models/Condition");

const store = (req, res) => {
  // console.log("req:", req);
  let condition = new Condition({
    ID: req.body.ID,
    mode: req.body.mode,
    soilState: {
      state: req.body.soilState.state,
      value: req.body.soilState.value,
    },
    tempState: {
      state: req.body.tempState.state,
      value: req.body.tempState.value,
    },
    waterState: {
      state: req.body.waterState.state,
      value: req.body.waterState.value,
    },
    waterTimeState: {
      state: req.body.waterTimeState.state,
      startDate: req.body.waterTimeState.startDate,
      endDate: req.body.waterTimeState.endDate,
    },
  });
  condition
    .save()
    .then((response) => {
      res.json({
        message: "Data added successfully!",
      });
    })
    .catch((error) => {
      res.json({
        error,
      });
      console.log(error);
    });
};
const getCondition = (req, res) => {
  let ID = req.params.ID;
  console.log("ID:", ID);
  Condition.find({ ID: ID })
    .limit(1)
    .sort({ createdAt: -1 })
    .then((response) => {
      res.json({ response });
    })
    .catch((error) => {
      res.json({ message: "An occured Error" });
      console.log(error);
    });
};
module.exports = {
  store,
  getCondition,
};
