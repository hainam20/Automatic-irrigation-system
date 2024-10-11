import React, { useEffect, useRef } from "react";
import moment from "moment";
import { DatePicker, Space } from 'antd';
import dayjs from "dayjs";
const { RangePicker } = DatePicker;


const ModalInfo = (props) => {

    const { handleModalInfo, tempState, waterState, soilState, handleModalChange,
            handleSaveCondition, handleModeChange, mode, waterTimeState, handleChangeWaterTime } = props;
    
    const modalRef = useRef(null);
    
    const onChange = (value, dateString) => {
        console.log(value);
        handleChangeWaterTime(dateString);
    };

    const onOk = (value) => {
    
    };

    return (
        <div className="fixed w-screen h-screen top-0 left-0 bottom-0 right-0 bg-[rgb(89,89,89)] bg-opacity-90 flex justify-center items-center drop-shadow-2xl shadow-2xl z-[120]">
            <div ref={modalRef} className="bg-white w-3/4 h-fit grid grid-cols-3 rounded-lg p-6 relative">
                <div className="h-fit mr-2 px-2 pt-4 pb-9 bg-white border border-[rgb(212,212,212)] border-t-[5px] border-t-[rgb(7,120,254)]">
                    <div className="text-sm p-3 mb-4">Select mode</div>

                    <div class="flex items-center mb-4">
                        <input id="country-option-1" type="radio" value="Automatic" class="w-4 h-4 border-gray-300 focus:ring-2 focus:ring-blue-300 dark:focus:ring-blue-600 dark:focus:bg-blue-600 dark:bg-gray-700 dark:border-gray-600" 
                            onChange={(e) => handleModeChange(e.target.value)}
                            checked={mode === "Automatic"}
                        />
                         <label for="country-option-1" class="block ms-2  text-sm  ">
                                Automatic
                        </label>
                    </div>
                    <div class="flex items-center mb-4">
                        <input id="country-option-1" type="radio" value="Manual" class="w-4 h-4 border-gray-300 focus:ring-2 focus:ring-blue-300 dark:focus:ring-blue-600 dark:focus:bg-blue-600 dark:bg-gray-700 dark:border-gray-600" 
                            onChange={(e) => handleModeChange(e.target.value)}
                            checked={mode === "Manual"}
                        />
                         <label for="country-option-1" class="block ms-2  text-sm ">
                                Manual
                        </label>
                    </div>
                </div>
                    
                <div className="h-fit px-2 py-4 bg-white border border-[rgb(212,212,212)] border-t-[5px] border-t-[rgb(219,69,55)]">
                    <div className="text-sm p-3 mb-4">Water when soil measure is less than</div>
                    <div className="text-3xl mb-4 text-center text-blue-600 font-semibold relative">
                        {tempState.value}%
                        <div className="absolute left-3 -top-[45%] text-center translate-y-1/2">
                            <label class="relative inline-flex items-center me-5 cursor-pointer">
                                <input 
                                    type="checkbox" 
                                    checked={tempState.state}
                                    class="sr-only peer"
                                    onChange={() => handleModalChange('tempState', true)}
                                />

                                <div class="w-11 h-6 bg-gray-200 rounded-full peer dark:bg-gray-700 peer-focus:ring-4 dark:peer-focus:ring-red-800 peer-checked:after:translate-x-full rtl:peer-checked:after:-translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-0.5 after:start-[2px] after:bg-white after:border-gray-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all dark:border-gray-600 peer-checked:bg-red-600"></div>
                            </label>
                        </div>
                    </div>
                    <div className="p-2 relative">
                        <input id="default-range" type="range" onChange={(e) => handleModalChange('tempState', false, e?.target?.value)} value={tempState.value} min="0" max="100" class="w-full h-2 bg-gray-200 rounded-lg appearance-none cursor-pointer dark:bg-white-700" />
                        <div className="absolute text-sm left-0">0%</div>
                        <div className="absolute text-sm right-0">100%</div>
                    </div>
                </div>
                <div className="h-fit px-2 py-4 mx-2 bg-white border border-[rgb(212,212,212)] border-t-[5px] border-t-[rgb(249,176,12)]">
                    <div className="text-sm p-3 mb-4">Don't water when soil measure more than </div>
                    <div className="text-3xl mb-4 text-center text-blue-600 font-semibold relative">
                        {soilState.value}%
                        <div className="absolute left-3 -top-[45%] text-center translate-y-1/2">
                            <label class="relative inline-flex items-center me-5 cursor-pointer">
                                <input 
                                    type="checkbox" 
                                    checked={soilState.state}
                                    class="sr-only peer"
                                    onChange={() => handleModalChange('soilState', true)}
                                />

                                <div class="w-11 h-6 bg-gray-200 rounded-full peer dark:bg-gray-700 peer-focus:ring-4 dark:peer-focus:ring-yellow-800 peer-checked:after:translate-x-full rtl:peer-checked:after:-translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-0.5 after:start-[2px] after:bg-white after:border-gray-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all dark:border-gray-600 peer-checked:bg-yellow-600"></div>
                            </label>
                        </div>
                    </div>
                    <div className="p-2 relative">
                        <input id="default-range" type="range" value={soilState.value} onChange={(e) => handleModalChange('soilState', false, e.target.value)}class="w-full h-2 bg-gray-200 rounded-lg appearance-none cursor-pointer dark:bg-white-700" />
                        <div className="absolute text-sm left-0">0%</div>
                        <div className="absolute text-sm right-0">100%</div>
                    </div>
                </div>
                <div className="h-fit px-2 py-4 mt-2 mx-2 bg-white border border-[rgb(212,212,212)] border-t-[5px] border-t-[rgb(37,184,82)]">
                    <div className="text-sm p-3 mb-4">Water when today's forecast temparature is more than</div>
                    <div className="text-3xl mb-4 text-center text-blue-600 font-semibold relative">
                        {waterState.value}
                        <div className="absolute left-3 -top-[45%] text-center translate-y-1/2">
                            <label class="relative inline-flex items-center me-5 cursor-pointer">
                                <input 
                                    type="checkbox" 
                                    checked={waterState.state}
                                    class="sr-only peer"
                                    onChange={() => handleModalChange('waterState', true)}
                                />

                                <div class="w-11 h-6 bg-gray-200 rounded-full peer dark:bg-gray-700 peer-focus:ring-4 dark:peer-focus:ring-green-800 peer-checked:after:translate-x-full rtl:peer-checked:after:-translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-0.5 after:start-[2px] after:bg-white after:border-gray-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all dark:border-gray-600 peer-checked:bg-green-600"></div>
                            </label>
                        </div>
                        <div className="p-2 relative text-black">
                            <input id="default-range" type="range" value={waterState.value} onChange={(e) => handleModalChange('waterState', false, e.target.value)} min="0" max="50" class="w-full h-2 bg-gray-200 rounded-lg appearance-none cursor-pointer dark:bg-white-700" />
                            <div className="absolute text-sm left-0">0°C</div>
                            <div className="absolute text-sm right-0">50°C</div>
                        </div>
                    </div>
                </div>

                <div className="h-fit px-2 py-4 mt-2 bg-white border border-[rgb(212,212,212)] border-t-[5px] border-t-[rgb(37,184,82)]">
                    <div className="text-sm p-3">Water when today's forecast temparature is more than</div>
                    <div className="text-3xl mb-4 flex flex-col justify-between text-blue-600 font-semibold relative">
                        <div className="ml-3">
                            <label class="relative inline-flex items-center me-5 cursor-pointer">
                                <input 
                                    type="checkbox" 
                                    checked={waterTimeState.state}
                                    class="sr-only peer"
                                    onChange={handleChangeWaterTime}
                                />

                                <div class="w-11 h-6 bg-gray-200 rounded-full peer dark:bg-gray-700 peer-focus:ring-4 dark:peer-focus:ring-green-800 peer-checked:after:translate-x-full rtl:peer-checked:after:-translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-0.5 after:start-[2px] after:bg-white after:border-gray-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all dark:border-gray-600 peer-checked:bg-green-600"></div>
                            </label>
                        </div>
                        <div className="p-2 text-black">
                            <Space direction="vertical" size={12}>
                                <RangePicker
                                    showTime={{
                                        format: 'HH:mm',
                                    }}
                                    format="YYYY-MM-DD HH:mm"
                                    onChange={onChange}
                                    onOk={onOk}
                                    value={[dayjs(waterTimeState.startDate, 'YYYY-MM-DD HH:mm'), dayjs(waterTimeState.endDate, 'YYYY-MM-DD HH:mm')]}
                                />
                            </Space>
                        </div>
                    </div>
                </div>
                
                <div className="absolute bottom-0 right-0">
                    <button onClick={handleModalInfo} type="button" class="focus:outline-none text-white bg-red-700 hover:bg-red-800 focus:ring-4 focus:ring-red-300 font-medium rounded-lg text-sm px-5 py-2.5 me-2 mb-2 dark:bg-red-600 dark:hover:bg-red-700 dark:focus:ring-red-900">Cancel</button>
                    <button onClick={handleSaveCondition} type="button" class="text-white bg-blue-700 hover:bg-blue-800 focus:ring-4 focus:ring-blue-300 font-medium rounded-lg text-sm px-5 py-2.5 me-2 mb-2 dark:bg-blue-600 dark:hover:bg-blue-700 focus:outline-none dark:focus:ring-blue-800">Save</button>
                </div>
                
            </div>
        </div>
    );
};

export default ModalInfo;