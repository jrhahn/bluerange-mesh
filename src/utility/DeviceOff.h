////////////////////////////////////////////////////////////////////////////////
// /****************************************************************************
// **
// ** Copyright (C) 2015-2022 M-Way Solutions GmbH
// ** Contact: https://www.blureange.io/licensing
// **
// ** This file is part of the Bluerange/FruityMesh implementation
// **
// ** $BR_BEGIN_LICENSE:GPL-EXCEPT$
// ** Commercial License Usage
// ** Licensees holding valid commercial Bluerange licenses may use this file in
// ** accordance with the commercial license agreement provided with the
// ** Software or, alternatively, in accordance with the terms contained in
// ** a written agreement between them and M-Way Solutions GmbH.
// ** For licensing terms and conditions see https://www.bluerange.io/terms-conditions. For further
// ** information use the contact form at https://www.bluerange.io/contact.
// **
// ** GNU General Public License Usage
// ** Alternatively, this file may be used under the terms of the GNU
// ** General Public License version 3 as published by the Free Software
// ** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
// ** included in the packaging of this file. Please review the following
// ** information to ensure the GNU General Public License requirements will
// ** be met: https://www.gnu.org/licenses/gpl-3.0.html.
// **
// ** $BR_END_LICENSE$
// **
// ****************************************************************************/
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <PrimitiveTypes.h>

/**
 * The DeviceOff can be used for turning device off and saving energy.
 */
class DeviceOff
{
    public:
        //Can be set to a time in Ds to automatically power off the device
        u32 autoPowerOffTimeDs = 0;

        //Can be set to a time in Ds to automatically power off the device
        //but only if it is unenrolled
        u32 autoPowerOffIfUnenrolledTimeDs = 0;

        DeviceOff();
        void HandleReset();
        void TimerHandler(u16 passedTimeDs);


    private:
        u32 powerButtonPressedTimeDs = 0;

        bool IsPowerButtonPressed();
        bool CheckPowerButtonLongPress(u32 delayMs);
        void PrepareSystemOff();
        void GotoSystemOff();
};
