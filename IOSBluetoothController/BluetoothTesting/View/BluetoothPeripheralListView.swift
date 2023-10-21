//
//  BluetoothPeripheralListView.swift
//  BluetoothTesting
//
//  Created by Max Chou on 8/10/23.
//

import SwiftUI

struct BluetoothPeripheralListView: View {
    @ObservedObject var bluetoothViewModel: BluetoothViewModel
    
    var body: some View{
        Text("Discoverd Peripherals")
        List(bluetoothViewModel.peripheralNames, id: \.self){
                name in
                    Text(name)
            }
        
      
    }
    
}
