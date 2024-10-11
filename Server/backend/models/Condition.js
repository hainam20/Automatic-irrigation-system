const mongoose = require("mongoose");
/*data = {
  tempState: { state: true, value: 25 },
  soilState: { state: false, value: 20 },
  waterState: { state: false, value: 20 },
};*/
const stateSchema = new mongoose.Schema({
  state: {
    type: Boolean,
    required: true,
  },
  value: {
    type: Number,
    required: true,
  },
});
const timeStateSchema = new mongoose.Schema({
  state: { type: Boolean, required: true },
  startDate: { type: String, required: true },
  endDate: { type: String, required: true },
});
const ConditionSchema = new mongoose.Schema(
  {
    ID: {
      type: Number,
    },
    tempState: {
      type: stateSchema,
      required: true,
    },
    soilState: {
      type: stateSchema,
      required: true,
    },
    waterState: {
      type: stateSchema,
      required: true,
    },
    mode: { type: String, required: true },
    waterTimeState: { type: timeStateSchema, required: true },
  },

  { timestamps: true }
);

const Condition = mongoose.model("Condition", ConditionSchema);

module.exports = Condition;
