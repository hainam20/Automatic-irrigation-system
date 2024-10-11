import React, { useState, useEffect } from 'react';
import axios from 'axios';
import moment from 'moment';
import { Table } from 'antd';
import socket from "../../socket.js"
import Relay from './Relay';
import ModalInfo from './ModalInfo';

import { ReactComponent as IconControl } from '../../assets/iconControl.svg';

import "./Control.css";

const columns = [
    {
        title: 'Relay',
        dataIndex: 'ID',
        key: 'ID',
        render: (text) => <a>{text + 1}</a>,
    },
    {
        title: 'Status',
        dataIndex: 'status',
        key: 'status',
        render: (text) => <a>{text === true ? "On" : "OFF"}</a>
    },
    {
        title: 'Time',
        dataIndex: 'createdAt',
        key: 'createdAt',
        render: (text) => <a>{moment(text).format('h:mm a, MMMM Do YYYY')}</a>,
    },
];


const Control = () => {
    const [relay, setRelay] = useState([]);
    useEffect(() =>{
        const fetchControls = async () => {
            try {
                const response = await axios.get('http://localhost:5000/api/control/relay');
                const updatedRelay = response.data.data.map(item => ({
                    ...item,
                    title: `Relay ${item.ID + 1}` 
                }));
                setRelay(updatedRelay);
            } catch (error) {
                console.log(error);
            }
        };
        fetchControls();
    }, []);


    const [relayHistory, setRelayHistory] = useState([]);
    const [state, setState] = useState({
        tempState: {
            state: true,
            value: 25,
        },
        soilState: {
            state: false,
            value: 20,
        },
        waterState: {
            state: false,
            value: 20,
        },

        waterTimeState: {
            state: false,
            startDate: '',
            endDate: '',
        },
        currId: '',
        isVisibleModalInfo: false,
        mode: 'Manual',
    });

// New Update
    useEffect(() => {
            relay.forEach((relayItem) => {
                if (relayItem.status !== relayItem.prevStatus) {
                    socket.emit('status_Relay', { "ID": relayItem.ID, "status": relayItem.status });
                    relayItem.prevStatus = relayItem.status;
                }
            });
    
    }, [relay]);

    useEffect(() => {
        const visibleModalData = async () => {
            if(state.isVisibleModalInfo === true) {
                try {
                    const response = await axios.get(`http://localhost:5000/api/condition/getcondition/${state.currId}`);
                    const { tempState, soilState, waterState, ID, mode, waterTimeState } = response.data.response[0];
                    if(state.currId === ID)
                    {
                        setState(prev => ({...prev, tempState: tempState,soilState: soilState, 
                                            waterState: waterState, mode: mode, waterTimeState: waterTimeState})); 
                    }
                } catch (error) {
                    console.log(error);
                }
            }
        }
        visibleModalData();
    }, [state.isVisibleModalInfo]);
    useEffect((() => {
        socket.on('relay_data', (data) => {
            setRelayHistory(data);
        });
    }))
    useEffect((() => {
        socket.on('relay', (data) => {
            console.log(data); 
        const { ID, status } = data;
        updateRelayValue(ID,status );
    });
    }),)
    const updateRelayValue = (id, newRelayValue) => {
        setRelay(prevObjects =>
          prevObjects.map(obj =>
            obj.ID === id ? { ...obj, status: newRelayValue } : obj
          )
        );
      };
    const handleChangeStatus = (index) => {
        const updatedRelay  = relay.map(obj => {
            try {
                if (obj.ID === index){
                    return {...obj, status: !relay[index].status}
                }
                return obj;              
            } catch (error) {
                console.log(error);
            }
        })
        setRelay(updatedRelay)
        //socket.emit('status_Relay', index);
    };

 
    const handleModalInfo = async (index) => { 
        setState(prev => ({...prev, currId: index,isVisibleModalInfo: !prev.isVisibleModalInfo}));
    };

    const handleModeChange = async (selectedMode) => { 
        setState(prev => ({...prev, mode: selectedMode}));
    };

    const handleSaveCondition = async () => {
        try {
            const { tempState, soilState, waterState, currId, mode, waterTimeState } = state;

            const data = {
                ID: currId,
                tempState: tempState,
                soilState: soilState,
                waterState: waterState,
                mode: mode,
                waterTimeState: waterTimeState,
            };
            console.log(data);
            const response = await axios.post(`http://localhost:5000/api/condition/store`,data);
            console.log(response.data);
        } catch (error) {
            console.log(error);
        }

        handleModalInfo();
    }


    const handleModalChange = (type, isState, value) => {
        if (isState) {
            setState(prev => ({...prev, [type]: {value: prev[type].value, state: !prev[type].state}}));
        } else {
            setState(prev => ({...prev, [type]: {value: Number(value), state: prev[type].state}}));
        }
    };
    
    const handleChangeWaterTime = (dateString) => {
        setState((prevState) => ({
            ...prevState,
            waterTimeState: {
                ...prevState.waterTimeState,
                state: !prevState.waterTimeState.state, // Toggle checkbox state
                startDate: dateString[0] || prevState.waterTimeState.startDate, // Update startDate if available
                endDate: dateString[1] || prevState.waterTimeState.endDate, // Update endDate if available
            },
        }));
    };

    // useEffect(() => {
    //     console.log(state.waterTimeState);
    // },[state.waterTimeState])

    return (
        <>
            <div className='bg-white w-full h-full p-3 rounded-md shadow-xl relative overflow-y-auto'>
                <div className='w-full flex justify-between items-center mb-3'>
                    <div className='flex items-center'>
                        <IconControl className='mr-4'/>
                        <div className='text-lg font-semibold text-[rgb(23,24,59)]'>Control</div>
                    </div>
                    <div className='text-lg font-bold tracking-widest text-green-400'>

                    </div>
                </div>
                {relay.map((item, index) => {
                    return (
                        <div className='py-1 px-4' key={`relay-${index}`}>
                            <Relay data={item} index = {index} handleChangeStatus={() => handleChangeStatus(index)} handleModalInfo={handleModalInfo}/>
                        </div>
                    )
                })}
                {state.isVisibleModalInfo && (
                    <ModalInfo
                        tempState={state.tempState}
                        waterState={state.waterState}
                        soilState={state.soilState}
                        waterTimeState={state.waterTimeState}
                        mode={state.mode}
                        handleModalInfo={handleModalInfo}
                        handleModalChange={handleModalChange}
                        handleModeChange={handleModeChange}
                        handleSaveCondition={handleSaveCondition}
                        handleChangeWaterTime={handleChangeWaterTime}
                    />
                )}
                <div className='w-full flex flex-col mt-4'>
                    {/* <div className='mb-4'>History: </div> */}
                    <div className='w-full flex'>
                        {/* <Empty /> */}
                        <Table 
                            dataSource={relayHistory} 
                            columns={columns} 
                            pagination={false}
                            className='w-full'
                        />
                    </div>
                </div>
            </div>
        </>
    );
};

export default Control;