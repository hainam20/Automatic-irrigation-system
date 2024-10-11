const express = require("express");
const router = express.Router();

const LoraDataController = require("../controllers/LoraDataController");

router.get("/", LoraDataController.index);
router.post("/show", LoraDataController.show);
router.post("/store", LoraDataController.store);
router.post("/update", LoraDataController.update);
router.post("/delete", LoraDataController.destroy);
router.get("/getchartdata", LoraDataController.getRecentDataGroupedByUserId);
module.exports = router;
