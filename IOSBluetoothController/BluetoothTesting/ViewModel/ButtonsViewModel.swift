//
//  ButtonsViewModel.swift
//  BluetoothTesting
//
//  Created by Max Chou on 8/14/23.
//

import SwiftUI
import os

enum  CommandID: Int16, CaseIterable {
    case Standby = 1
    case LiftUp = 2
    case DropDown = 3
    case RollLeft = 4
    case RollRight = 5
    case ShutDown = 2000
    case ToggleOn = 2001
    case SafetyOn = 2002
    case SafetyOff = 2003
    case Calibrated = 2004
    case MaxThrottle = 2005
    case MinThrottle = 2006
}

struct ButtonsViewModel {
    @ObservedObject var bluetoothViewModel: BluetoothViewModel

    func produceCommandButtonViewModels() -> [CommandButtonViewModel] {
        [
            CommandButtonViewModel(buttonName: "Shut Down", commandID: CommandID.ShutDown.rawValue, bluetoothViewModel),
            CommandButtonViewModel(buttonName: "Toggle On", commandID: CommandID.ToggleOn.rawValue, bluetoothViewModel),
            CommandButtonViewModel(buttonName: "Safety On", commandID: CommandID.SafetyOn.rawValue, bluetoothViewModel),
            CommandButtonViewModel(buttonName: "Safety Off", commandID: CommandID.SafetyOff.rawValue, bluetoothViewModel),
            CommandButtonViewModel(buttonName: "Calibrated", commandID: CommandID.Calibrated.rawValue, bluetoothViewModel),
            CommandButtonViewModel(buttonName: "Max Throttle (Calibration use only)", commandID: CommandID.MaxThrottle.rawValue, bluetoothViewModel),
            CommandButtonViewModel(buttonName: "Min Throttle (Calibration use only)", commandID: CommandID.MinThrottle.rawValue, bluetoothViewModel),
        ]
    }
    
      
//    CommandButtonViewModel(buttonName: "Standby", commandID: CommandID.Standby.rawValue, bluetoothViewModel),
//    CommandButtonViewModel(buttonName: "Lift Up", commandID: CommandID.LiftUp.rawValue, bluetoothViewModel ),
//    CommandButtonViewModel(buttonName: "Drop Down", commandID: CommandID.DropDown.rawValue, bluetoothViewModel),
//    CommandButtonViewModel(buttonName: "Roll Left", commandID: CommandID.RollLeft.rawValue, bluetoothViewModel),
//    CommandButtonViewModel(buttonName: "Roll Right", commandID: CommandID.RollRight.rawValue, bluetoothViewModel),
}
