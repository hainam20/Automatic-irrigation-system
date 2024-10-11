import React, { useEffect, useState } from "react";

import { ReactComponent as IconDevice } from '../../../assets/iconDevice.svg';



const Relay = (props) => {

    const { data, handleModalInfo, handleChangeStatus, index } = props;
    const [state, setState] = useState({
        status: data.status,
    });
    useEffect(() => {
        if (data) {
            setState(prev => ({...prev, status: data.status}));
            
        }
    },[data]);
    return (
        <div
            onClick={()=> handleModalInfo(index)}
            className="w-full h-[90px] bg-[rgb(82,127,244)] opacity-100 hover:opacity-80 transition-opacity duration-300 rounded-xl flex items-center cursor-pointer justify-between px-4"
        >
            <div className="flex items-center h-full">
                <IconDevice className="transform scale-75 mr-2"/>
                <div className="flex flex-col">
                    <div className="text-white font-medium tracking-wider">{data.title}</div>
                    <div className="text-white opacity-50">{data.status ? 'Relay is ON' : 'Relay is OFF'}</div>
                </div>
            </div>
            <div className="h-full flex items-center">
                <label class="relative inline-flex items-center me-5 cursor-pointer">
                    <input 
                        type="checkbox" 
                        checked={data.status}
                        value={true}
                        defaultChecked={data.status}
                        onChange={handleChangeStatus} 
                        class="sr-only peer"
                    />

                    <div class="w-11 h-6 bg-gray-200 rounded-full peer dark:bg-gray-700 peer-focus:ring-4 peer-focus:ring-green-300 dark:peer-focus:ring-green-800 peer-checked:after:translate-x-full rtl:peer-checked:after:-translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-0.5 after:start-[2px] after:bg-white after:border-gray-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all dark:border-gray-600 peer-checked:bg-green-600"></div>
                    <span class="ms-3 text-sm font-medium text-gray-900 dark:text-gray-300">{data.status ? 'On' : 'Off'}</span>
                </label>
            </div>
        </div>
    );
};

export default Relay;