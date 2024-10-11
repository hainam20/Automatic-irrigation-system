const express = require("express");
const router = express.Router();

const ConditionController = require("../controllers/ConditionController");

router.post("/store", ConditionController.store);
router.get("/getcondition/:ID", ConditionController.getCondition);

module.exports = router;
