import React, { useState, useEffect, useRef } from 'react';
import io from 'socket.io-client';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';


import { ReactComponent as IconClose } from '../../assets/iconClose.svg';

import "./chart.css";

import socket from "../../socket.js"

export default function Chart(props) {

    const { handleCloseChart, data } = props;
    const [chartData, setChartData] = useState([]);
    const MAX_DATA_POINTS = 20;

    const modalRef = useRef(null);

    useEffect(() => {
        const handleClickOutside = (event) => {
            if (modalRef.current && !modalRef.current.contains(event.target)) {
                handleCloseChart();
            }
        };

        document.addEventListener('mousedown', handleClickOutside);

        return () => {
            document.removeEventListener('mousedown', handleClickOutside);
        }
    },[]);

    useEffect(() => {
        socket.on("connect", () => {
            console.log('Connected to Flask server');
        });

        socket.on('mqtt_data', (data) => {
            console.log('Received data from server', data);
            setChartData((prevData) => {
                const newData = [...prevData, data];
                if (newData.length > MAX_DATA_POINTS) {
                    newData.shift(); 
                }

                return newData;
            });
        });
    }, []);
 
    return (
        <div ref={modalRef} className="chart bg-white rounded-md w-[800px] h-[500px] relative">
            <IconClose 
                className='absolute top-2 right-2 cursor-pointer icon-close'
                onClick={handleCloseChart}
            />
            <h3 className="text-center mb-5">Data Analys</h3>
            {/* <ResponsiveContainer width="100%" aspect={3 / 1}>
                <LineChart data={chartData}>
                <XAxis dataKey="createdAt" stroke="#5550bd" angle={0} tickFormatter={(createdAt) => format(new Date(createdAt), 'HH:mm:ss') }/>
                <YAxis domain={[0,100]} type= "number"/>
                <Line type="monotone" dataKey="temp" stroke="red" strokeWidth={3}/>
                <Line type="monotone" dataKey="hum" stroke="blue" strokeWidth={3}/>
                <Line type="monotone" dataKey="adc_val" stroke="gray" strokeWidth={3}/>
                <CartesianGrid stroke="#e0dfdf" strokeDasharray="5 5"/>
                <Legend verticalAlign="top" layout="horizontal"/>
                <Tooltip />
                </LineChart>
            </ResponsiveContainer> */}
            <ResponsiveContainer width='100%' height='85%'>
                <LineChart
                    width={700}
                    height={500}
                    data={data}
                    margin={{
                        top: 5,
                        right: 30,
                        left: 20,
                        bottom: 5,
                    }}
                >
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis dataKey="day" />
                    <YAxis yAxisId="right" unit="°C" />
                    <YAxis yAxisId="left" orientation="right" unit="%" />
                    <Tooltip />
                    <Legend />
                    <Line yAxisId="right" type="monotone" dataKey="Temperature" stroke="#8884d8" />
                    <Line yAxisId="left" type="monotone" dataKey="Humidity" stroke="#82ca9d" />
                    <Line yAxisId="left" type="monotone" dataKey="Soil" stroke="#2da7e4" />
                </LineChart>
            </ResponsiveContainer>
        </div>
    );
};

