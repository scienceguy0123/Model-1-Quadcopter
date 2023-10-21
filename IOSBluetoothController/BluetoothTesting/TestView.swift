////
////  TestView.swift
////  BluetoothTesting
////
////  Created by Max Chou on 9/23/23.
////
//
//import Foundation
//import SwiftUI
//
//struct TestView:UIViewControllerRepresentable {
//    @Binding var climbPower: Double
//    @ObservedObject var bluetoothViewModel: BluetoothViewModel
//    func makeUIViewController(context: Context) -> ContentViewController {
//        let vc = ContentViewController(climbPower:$climbPower, bluetoothViewModel: bluetoothViewModel)
//        
//        return vc
//    }
//    
//    func updateUIViewController(_ uiViewController: ContentViewController, context: Context) {
//        
//    }
//    
//    typealias UIViewControllerType = ContentViewController
//    
//    
//    
//}
