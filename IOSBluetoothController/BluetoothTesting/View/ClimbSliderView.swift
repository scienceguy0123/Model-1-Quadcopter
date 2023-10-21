//
//  ClimbSliderView.swift
//  BluetoothTesting
//
//  Created by Max Chou on 9/12/23.
//

import SwiftUI

struct ClimbSliderView: View {
    @ObservedObject var bluetoothViewModel: BluetoothViewModel
    @Binding var climbPower: Double 

    var body: some View {
        VStack {
            Slider(value: $climbPower,
                   in: 0...1023,
                   step: 1
            
            )
//            Text("\(climbPower)")
        }
    }
}
