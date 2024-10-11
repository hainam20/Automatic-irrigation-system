import { io } from "socket.io-client";

const socket = io("localhost:5000/", {
  transports: ["websocket"],
  cors: {
    origin: "http://localhost:3000/",
  },
});
export default socket;
