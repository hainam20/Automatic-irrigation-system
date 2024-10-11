import React from 'react';
import NotificationsNoneIcon from '@mui/icons-material/NotificationsNone';
import LanguageIcon from '@mui/icons-material/Language';
import SettingsIcon from '@mui/icons-material/Settings';

import "./topbar.css";

export default function Topbar() {
    return (
        <div className="topba border-b">
            <div className="topbarWrapper">
                <div className="topLeft">
                    <span className="logo">ADMIN</span>
                </div>
                <div className="topRight">
                    <div className="topbarIcons">
                        <NotificationsNoneIcon/>
                    </div>
                    <div className="topbarIcons">
                        <LanguageIcon/>
                    </div>
                    <div className="topbarIcons">
                        <SettingsIcon/>
                    </div>
                </div>
            </div>
        </div>
    );
};
