//
//  CommandButtonViewModel.swift
//  BluetoothTesting
//
//  Created by Max Chou on 7/9/23.
//

import SwiftUI
import os


struct CommandButtonViewModel: Identifiable {
    let id: UUID
    var buttonName: String
    var commandID: Int16
    @ObservedObject var bluetoothViewModel: BluetoothViewModel
    
    init(id: UUID = UUID(), buttonName:String, commandID:Int16, _ bluetoothViewModel:BluetoothViewModel) {
        self.id = id
        self.buttonName = buttonName
        self.commandID = commandID
        self.bluetoothViewModel = bluetoothViewModel
    }
    
    func click() -> Void {
        os_log("Clicked %s button, with command ID %d", self.buttonName, self.commandID)
        self.bluetoothViewModel.writeData(self.commandID)
 
    }
    
}
