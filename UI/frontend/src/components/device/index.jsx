import React, {useState, useEffect} from "react";

import { Button } from 'antd';
import { FolderOpenOutlined } from '@ant-design/icons';

import { ReactComponent as IconDevice } from '../../assets/iconDevice.svg';
import { ReactComponent as IconTemparature } from '../../assets/iconTemparature.svg';
import { ReactComponent as IconHumidity } from '../../assets/iconHumidity.svg';
import { ReactComponent as IconSoil } from '../../assets/iconSoil.svg';



const Device = (props) => {

    const { data, handleVisibleChart, index } = props;

    return (
        <div className="w-full h-[200px] bg-white rounded-lg shadow-xl p-3 relative flex cursor-pointer hover:scale-105 hover:transition-all">
            <div className="w-[50%]">
                <IconDevice />
                <div className="text-base font-normal tracking-widest mb-3">Device {data?.ID + 1}</div>
                <div className="">
                    <Button 
                        type="primary" 
                        icon={<FolderOpenOutlined />}
                        onClick={() => handleVisibleChart(data?.chart, index)}
                    > 
                        Open chart
                    </Button>
                </div>
            </div>
            <div className="w-[50%] flex flex-col justify-center">
                <div className="text-sm flex items-center mb-3 w-full">
                    <div className="flex flex-col items-center w-[50%]">
                        <IconTemparature className="mr-3"/>
                        <div className="">Temparature</div>
                    </div>
                    <div className="w-[50%] flex items-center justify-center text-lg">
                        {`${data?.temp}°`}
                    </div>
                </div>
                <div className="text-sm flex items-center mb-3 w-full">
                    <div className="flex flex-col items-center w-[50%]">
                        <IconHumidity className="mr-3"/>
                        <div className="">Humidity</div>
                    </div>
                    <div className="w-[50%] flex items-center justify-center text-lg">
                        {`${data?.hum}%`}
                    </div>
                </div>
                <div className="text-sm flex items-center mb-3 w-full">
                    <div className="flex flex-col items-center w-[50%]">
                        <IconSoil className="mr-3"/>
                        <div className="">Soil Measure</div>
                    </div>
                    <div className="w-[50%] flex items-center justify-center text-lg">
                        {`${data?.adc_val}%`}
                    </div>
                </div>
            </div>
        </div>
    );
};

export default Device;