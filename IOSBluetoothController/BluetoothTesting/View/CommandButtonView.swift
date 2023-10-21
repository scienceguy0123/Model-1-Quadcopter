//
//  CommandButton.swift
//  BluetoothTesting
//
//  Created by Max Chou on 7/9/23.
//
import SwiftUI
import os

struct CommandButtonView: View{
    var model:CommandButtonViewModel
    
    
    var body: some View{
        Button(action:self.model.click){
            Text(model.buttonName).padding(20)
        }
       
    }
}
//    @ObservedObject var bluetoothViewModel: BluetoothViewModel
//    let buttonViewModels: [CommandButtonViewModel] = CommandButtonViewModel.commandButtonViewModels
    
//    func click(_ command:String) -> () -> () {
//        return {
//            os_log("Clicked \(command) button")
//            bluetoothViewModel.writeData(command)
//        }
//    }
//    func produceCommandButtons(){
//        var buttonModels =
//        for command in CommandID.allCases{
//
//        }
//    }
//
