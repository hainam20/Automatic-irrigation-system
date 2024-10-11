const express = require("express");
const router = express.Router();

const ControlController = require("../controllers/Control");

router.get("/", ControlController.index);
router.post("/show", ControlController.show);
router.post("/delete", ControlController.destroy);
router.get("/relay", ControlController.findRelayData);
module.exports = router;
