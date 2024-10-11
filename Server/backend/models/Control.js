const mongoose = require("mongoose");

const Control = new mongoose.Schema(
  {
    ID: {
      type: Number,
      required: true,
    },
    status: {
      type: Boolean,
      require: true,
    },
  },
  { timestamps: true }
);

const ControlData = mongoose.model("Control", Control);

module.exports = ControlData;
