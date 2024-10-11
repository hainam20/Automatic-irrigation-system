const { response } = require("express");
const { error } = require("console");
const ControlData = require("../models/Control");
//Show the list of Lora Data

const index = (req, res, next) => {
  ControlData.find()
    .then((response) => {
      res.json({
        response,
      });
    })
    .catch((error) => {
      res.json({
        message: "An error occured",
      });
    });
};

// show single Lora data
const show = (req, res, next) => {
  let ControlDataID = req.body.ID;
  ControlData.findById(ControlDataID)
    .then((response) => {
      res.json({
        response,
      });
    })
    .catch((error) => {
      res.json({
        message: "An error Occured!",
      });
    });
};

// add new LoRa data
const store = (req, res, next) => {
  let controldata = new ControlData({
    ID: req.body.ID,
    Status: req.body.Status,
  });
  controldata
    .save()
    .then((response) => {
      res.json({
        message: "Data added successfully!",
      });
    })
    .catch((error) => {
      res.json({
        message: "An error Occured",
      });
    });
};
const destroy = (req, res, next) => {
  let ControlDataID = req.body.ID;
  ControlData.findByIdAndRemove(ControlDataID)
    .then(() => {
      res.json({
        message: "Data deleted succesfully !!",
      });
    })
    .catch((error) => {
      req.json({
        message: "An error occured !",
      });
    });
};
const findRelayData = async (req, res) => {
  try {
    const latestControls = await ControlData.aggregate([
      { $sort: { createdAt: -1 } },

      {
        $group: {
          _id: "$ID",
          latestDocument: { $first: "$$ROOT" },
        },
      },

      { $replaceRoot: { newRoot: "$latestDocument" } },

      { $sort: { ID: 1 } },
      // Project to remove  field
      { $project: { _id: 0, createdAt: 0, updatedAt: 0, __v: 0 } },
    ]);

    res.json({
      message: "Latest control data fetched successfully!",
      data: latestControls,
    });
  } catch (error) {
    res.status(500).json({
      message: "An error occurred!",
      error: error.message,
    });
  }
};

module.exports = {
  index,
  show,
  store,
  destroy,
  findRelayData,
};
