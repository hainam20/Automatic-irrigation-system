import React, { useState } from 'react';

import HomeIcon from '@mui/icons-material/Home';
import LeaderboardIcon from '@mui/icons-material/Leaderboard';

import "./sidebar.css";

export default function Sidebar() {

    const [state, setState] = useState({
        activeTab: 0,
    });

  return (
    <div className="sidebar border-r">
        <div className="sidebarWrapper">
            <div className="sidebarMenu">
                <ul className="sidebarList">
                    <li 
                        className={`sidebarListItem mb-4 !p-3 hover:text-white ${state.activeTab === 0 ? 'bg-[rgb(56,125,255)] text-white' : 'bg-white'}`}
                        onClick={() => setState(prev => ({...prev, activeTab: 0}))}
                    >
                        <HomeIcon className="sidebarIcon !mr-3"/>
                        Home
                    </li>
                    <li 
                        className={`sidebarListItem !p-3 hover:text-white ${state.activeTab === 1 ? 'bg-[rgb(56,125,255)] text-white' : 'bg-white'}`}
                        onClick={() => setState(prev => ({...prev, activeTab: 1}))}
                    >
                        <LeaderboardIcon className="sidebarIcon !mr-3"/>
                        Chart
                    </li>
                </ul>
            </div>
        </div>
    </div>
  );
};
