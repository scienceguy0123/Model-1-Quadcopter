//
//  CommandButtonViews.swift
//  BluetoothTesting
//
//  Created by Max Chou on 8/14/23.
//

import SwiftUI

struct CommandButtonsView: View{
    @ObservedObject var bluetoothViewModel: BluetoothViewModel
    var buttonsViewModel : ButtonsViewModel
    var commandButtonsViewModels : [CommandButtonViewModel]
    
    init( _ bluetoothViewModel:BluetoothViewModel){
        
        self.bluetoothViewModel = bluetoothViewModel
        self.buttonsViewModel = ButtonsViewModel(bluetoothViewModel: bluetoothViewModel)
        self.commandButtonsViewModels = buttonsViewModel.produceCommandButtonViewModels()
    }
    
    var body: some View{
        ForEach(self.commandButtonsViewModels){ model in
            CommandButtonView(model: model)
        }
    }
}
