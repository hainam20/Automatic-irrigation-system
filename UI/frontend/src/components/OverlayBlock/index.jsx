import React from "react";

const OverlayBlock = ({ children }) => {
    return (
        <div className="fixed w-screen h-screen top-0 left-0 bottom-0 right-0 bg-[rgb(89,89,89)] bg-opacity-90 flex justify-center items-center drop-shadow-2xl shadow-2xl z-10">
            {children}
        </div>
    );
};

export default OverlayBlock;