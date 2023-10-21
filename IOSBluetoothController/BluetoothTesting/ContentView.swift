//
//  ContentView.swift
//  BluetoothTesting
//
//  Created by Max Chou on 7/5/23.
//

import SwiftUI
import CoreBluetooth
import os


struct ContentView: View {
    @StateObject private var bluetoothViewModel = BluetoothViewModel()
    @State private var climbPower: Double = 0.00
    let timer = Timer.publish(every:0.3 , on:.main, in:.common).autoconnect()

    var body: some View {
        VStack{
//            CommandButtonsView(bluetoothViewModel)
            ClimbSliderView(bluetoothViewModel: bluetoothViewModel, climbPower:$climbPower)
//            BluetoothPeripheralListView(bluetoothViewModel: bluetoothViewModel)
            
        }.padding(50 )
//        TestView( climbPower:$climbPower, bluetoothViewModel: bluetoothViewModel)

        VStack{
            Text("\(climbPower)")
        }.onReceive(timer){ time in
            self.bluetoothViewModel.writeData(Int16(climbPower))
        }
           
        }
//            of: climbPower) {newValue in self.bluetoothViewModel.writeData(Int16(newValue))}
    }




struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
