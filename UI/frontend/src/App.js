import Topbar from "./components/topbar/Topbar";
import Sidebar from "./components/sidebar/Sidebar";
import Home from "./pages/home/Home";
import "./index.css";
import Control from "./components/control/Control";

function App() {
  return (
    <div className="!h-screen">
      <Topbar />
      <div className="flex h-full content-container">
        <Sidebar />
        <div className="flex w-full h-full">
          <Home />
          {/* <Control /> */}
        </div>
      </div>
    </div>
  );
}

export default App;
