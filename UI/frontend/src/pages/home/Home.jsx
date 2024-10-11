import React, { useEffect, useState } from 'react'
import io from 'socket.io-client'

import Device from '../../components/device';
import Chart from '../../components/chart/Chart';
import OverlayBlock from '../../components/OverlayBlock';
import Control from '../../components/control/Control';

import axios from 'axios';

import "./home.css";

import socket from "../../socket.js"

  const dummyData = [
    {
        "ID": "1",
        "temparature": 18,
        "humidity": 70,
        "soil": 40,
        "chart": [
            { name: 'day 1', Temparature: 18, Humidity: 90, Soil: 20 },
            { name: 'day 2', Temparature: 19, Humidity: 60, Soil: 10 },
            { name: 'day 3', Temparature: 20, Humidity: 80, Soil: 90 },
            { name: 'day 4', Temparature: 18, Humidity: 66, Soil: 10 },
            { name: 'day 5', Temparature: 16, Humidity: 12, Soil: 34 },
            { name: 'day 6', Temparature: 22, Humidity: 14, Soil: 15 },
            { name: 'day 7', Temparature: 19, Humidity: 15, Soil: 90 },
        ]
    },
    {
        "ID": "2",
        "temparature": 18,
        "humidity": 70,
        "soil": 40,
        "chart": [
            { name: 'day 1', Temparature: 12, Humidity: 45, Soil: 34 },
            { name: 'day 2', Temparature: 21, Humidity: 23, Soil: 45 },
            { name: 'day 3', Temparature: 12, Humidity: 54, Soil: 65 },
            { name: 'day 4', Temparature: 14, Humidity: 34, Soil: 45 },
            { name: 'day 5', Temparature: 15, Humid2ity: 65, Soil: 34 },
            { name: 'day 6', Temparature: 18, Humidity: 34, Soil: 23 },
            { name: 'day 7', Temparature: 12, Humidity: 34, Soil: 12 },
        ]
    },
    {
        "ID": "3",
        "temparature": 18,
        "humidity": 70,
        "soil": 40,
        "chart": [
            { name: 'day 1', Temparature: 13, Humidity: 43, Soil: 34 },
            { name: 'day 2', Temparature: 14, Humidity: 12, Soil: 23 },
            { name: 'day 3', Temparature: 23, Humidity: 56, Soil: 34 },
            { name: 'day 4', Temparature: 14, Humidity: 34, Soil: 23 },
            { name: 'day 5', Temparature: 19, Humidity: 65, Soil: 34 },
            { name: 'day 6', Temparature: 12, Humidity: 76, Soil: 45 },
            { name: 'day 7', Temparature: 22, Humidity: 45, Soil: 14 },
        ]
    },
];

export default function Home() {
    /**
     * Socket.io
     */
    const [testData, setData] = useState([]);
    const [state, setState] = useState({
        isVisibleChart: false,
        controlData: {},
        chartData: [],
    });
   
    useEffect(() => {
        socket.on("connect", () => {
            console.log('Socket Connect:', socket.connected);
        });
        socket.on('mqtt_data', (testData) => {
            console.log('Received new data:', testData);
            setData(testData);
        });
        socket.on("alert", (message) => {
            console.log(message);
            alert(message);
        })
     
    }, []);


    const handleVisibleChart  = async (data, index) => {
        let chart = [];
        
        try {
            const response = await axios.get(`http://localhost:5000/api/loradata/getchartdata`);
            chart = response.data?.response?.[index]?.dailyAverages;
            console.log(response.data.response);
        } catch (error) {
            console.error('Error fetching data:', error);
        }
        setState(prev => ({...prev, isVisibleChart: true, chartData: chart}));
        

    };


    const handleCloseChart = () => {
        setState(prev => ({...prev, isVisibleChart: false}));
    };

    return (
        <div className='home bg-[rgb(245,246,250)] w-full h-full flex'>
            <div className='h-full overflow-y-auto no-scrollbar p-5 w-[50%]'>
                {testData?.map((item, index) => {
                    return (
                        <div className="mb-3 device" key={index}>
                            <Device data={item} index={index} handleVisibleChart={handleVisibleChart}/>
                        </div>
                    )
                })}
            </div>
            <div className='w-[45%] h-full p-5'>
                <Control />
            </div>
            {state.isVisibleChart && (
                <OverlayBlock>
                    <Chart handleCloseChart={handleCloseChart} data={state.chartData}/>
                </OverlayBlock>
            )}
        </div>
    );
};
