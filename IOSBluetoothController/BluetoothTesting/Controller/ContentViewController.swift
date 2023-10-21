//
//  ContentViewController.swift
//  BluetoothTesting
//
//  Created by Max Chou on 9/23/23.

//import UIKit
//import os
//import SwiftUI
//
//class ContentViewController: UIViewController,ObservableObject {
//    @ObservedObject var bluetoothViewModel: BluetoothViewModel
//    @Binding var climbPower: Double //
//    
//    convenience init() {
//        self.init(bluetoothViewModel: bluetoothViewModel, climbPower:climbPower)
//        self.bluetoothViewModel = bluetoothViewModel
//        self.climbPower = climbPower
//    }
//    
//    init(bluetoothViewModel: BluetoothViewModel, climbPower: Double) {
//        self.bluetoothViewModel = bluetoothViewModel
//        self.climbPower = climbPower
//        super.init(nibName: nil, bundle: nil)
//    }
//    
//    // if this view controller is loaded from a storyboard, imageURL will be nil
//    
//    required init?(coder aDecoder: NSCoder) {
//        super.init(coder: aDecoder)
//    }
////
////    required init?(coder: NSCoder) {
////        fatalError("init(coder:) has not been implemented")
////    }
//  
//    override func viewDidLoad() {
//        super.viewDidLoad()
//
//        // scheduling a timer in repeat mode after each 2 seconds.
//        _ = Timer.scheduledTimer(timeInterval: 0.05,
//                                         target: self,
//                                         selector: #selector(handleTimerExecution),
//                                         userInfo: nil,
//                                         repeats: true)
//    }
//
//    @objc private func handleTimerExecution() {
//        os_log("timer executed... %d")
//        
//    }
//}
