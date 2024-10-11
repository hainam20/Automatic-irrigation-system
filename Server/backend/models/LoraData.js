const mongoose = require("mongoose");

const LoraDataSchema = new mongoose.Schema(
  {
    ID: {
      type: Number,
      required: true,
    },
    temp: {
      type: Number,
      required: true,
    },
    hum: {
      type: Number,
      required: true,
    },
    adc_val: {
      type: Number,
      required: true,
    },
  },
  { timestamps: true }
);

const LoraData = mongoose.model("LoraData", LoraDataSchema);

module.exports = LoraData;
