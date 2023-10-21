//
//  BluetoothViewModel.swift
//  BluetoothTesting
//
//  Created by Max Chou on 7/9/23.
//

import CoreBluetooth
import os

class BluetoothViewModel: NSObject, ObservableObject {
    private var centralManager: CBCentralManager!
    private var peripherals: [CBPeripheral] = []
    private var flightControllerServiceUUID : CBUUID = CBUUID(string: "19b10000-e8f2-537e-4f6c-d104768a1214")
    private var flightControllerCharacteristicUUID : CBUUID = CBUUID(string: "19b10001-e8f2-537e-4f6c-d104768a1214")
    @Published var peripheralNames: [String] = []
    
    var discoveredPeripheral: CBPeripheral?
    var transferCharacteristic: CBCharacteristic?
    var writeIterationsComplete = 0
    var connectionIterationsComplete = 0
    
    var data = Data()
    
    let defaultIterations = 20   // change this value based on test usecase
    
    override init() {
        super.init()
        self.centralManager = CBCentralManager(delegate: self, queue: .main)
    }
    
    /*
     * We will first check if we are already connected to our counterpart
     * Otherwise, scan for peripherals - specifically for our service's 128bit CBUUID
     */
    private func retrievePeripheral(){
        
        let connectedPeripherals: [CBPeripheral] = (centralManager.retrieveConnectedPeripherals(withServices: [flightControllerServiceUUID]))
        
        print("Found connected Peripherals with transfer service: %@", connectedPeripherals)
        
        if let connectedPeripheral = connectedPeripherals.last {
            os_log("Connecting to peripheral %@", connectedPeripheral)
            self.discoveredPeripheral = connectedPeripheral
            centralManager.connect(connectedPeripheral, options: nil)
        } else {
            // We were not connected to our counterpart, so start scanning
            centralManager.scanForPeripherals(withServices: [flightControllerServiceUUID],
                                              options: [CBCentralManagerScanOptionAllowDuplicatesKey: true])
        }
    }
    
    /*
     *  Call this when things either go wrong, or you're done with the connection.
     *  This cancels any subscriptions if there are any, or straight disconnects if not.
     *  (didUpdateNotificationStateForCharacteristic will cancel the connection if a subscription is involved)
     */
    private func cleanup() {
        // Don't do anything if we're not connected
        guard let discoveredPeripheral = discoveredPeripheral,
              case .connected = discoveredPeripheral.state else { return }
        
        for service in (discoveredPeripheral.services ?? [] as [CBService]) {
            for characteristic in (service.characteristics ?? [] as [CBCharacteristic]) {
                if characteristic.uuid == flightControllerCharacteristicUUID && characteristic.isNotifying {
                    // It is notifying, so unsubscribe
                    self.discoveredPeripheral?.setNotifyValue(false, for: characteristic)
                }
            }
        }
        
        // If we've gotten this far, we're connected, but we're not subscribed, so we just disconnect
        centralManager.cancelPeripheralConnection(discoveredPeripheral)
    }
    
    /*
     *  Write some test data to peripheral
     */
    public func writeData(_ command:Int16) {
        
        guard let discoveredPeripheral = discoveredPeripheral,
                let transferCharacteristic = transferCharacteristic
            else {
//            os_log("Missing either discoveredPeripheral or transferCharacteristic")
            return }
        
//        os_log("writing %d to peripheral", command)

        var packetData = Data()
        let array = withUnsafeBytes(of: command, Array.init)
        packetData.append(contentsOf: array)
//        os_log("%s", array.description)
//        os_log("%s", packetData.description)
        discoveredPeripheral.writeValue(packetData, for: transferCharacteristic, type: .withResponse)
        
//        os_log("written %d to peripheral", command)
        
        
        //        os_log("writing data %s to peripheral", command)
//

//        os_log("Has discoveredPeripheral, transferCharacteristic")
        // check to see if number of iterations completed and peripheral can accept more data
//        while writeIterationsComplete < defaultIterations && discoveredPeripheral.canSendWriteWithoutResponse {
//            data.append(contentsOf: [49])
//
//            let mtu = discoveredPeripheral.maximumWriteValueLength (for: .withResponse)
//            var rawPacket = [UInt8]()
//
//            let bytesToCopy: size_t = min(mtu, data.count)
//            data.copyBytes(to: &rawPacket, count: bytesToCopy)
//            let packetData = Data(bytes: &rawPacket, count: bytesToCopy)
//
//            let stringFromData = String(data: packetData, encoding: .utf8)
//            os_log("Writing %d bytes: %s", bytesToCopy, String(describing: stringFromData))
//
//            discoveredPeripheral.writeValue(packetData, for: transferCharacteristic, type: .withResponse)
//            os_log("Written value to peripheral")
//            writeIterationsComplete += 1
//            data.removeAll(keepingCapacity: false)
//
//        }
//
//        if writeIterationsComplete == defaultIterations {
//            // Cancel our subscription to the characteristic
//            discoveredPeripheral.setNotifyValue(false, for: transferCharacteristic)
//        }
    }
}
    
    
extension BluetoothViewModel: CBCentralManagerDelegate {
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
            
        case .poweredOn:
            // ... so start working with the peripheral
            os_log("CBManager is powered on")
            retrievePeripheral()
            
        case .poweredOff:
            os_log("CBManager is powered off")
            return
            
        case .resetting:
            os_log("CBManager is resetting")
            return
        case .unauthorized:
            // In a real app, you'd deal with all the states accordingly
            os_log("CBManager is unauthorized")
            return
        case .unknown:
            os_log("CBManager state is unknown")
            // In a real app, you'd deal with all the states accordingly
            return
        case .unsupported:
            os_log("Bluetooth is not supported on this device")
            // In a real app, you'd deal with all the states accordingly
            return
        @unknown default:
            os_log("A previously unknown central manager state occurred")
            // In a real app, you'd deal with yet unknown cases that might occur in the future
            return
        }
        
    }
    
    
    /*
     *  This callback comes whenever a peripheral that is advertising the transfer serviceUUID is discovered.
     *  We check the RSSI, to make sure it's close enough that we're interested in it, and if it is,
     *  we start the connection process
     */
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        
        // Reject if the signal strength is too low to attempt data transfer.
        // Change the minimum RSSI value depending on your app’s use case.
        guard RSSI.intValue >= -100
        else {
            os_log("Discovered perhiperal not in expected range, at %d", RSSI.intValue)
            return
        }
        
        os_log("Discovered %s at %d", String(describing: peripheral.name), RSSI.intValue)
        
        // Device is in range - have we already seen it?
        if discoveredPeripheral != peripheral {
            
            // Save a local copy of the peripheral, so CoreBluetooth doesn't get rid of it.
            discoveredPeripheral = peripheral
            
            // And finally, connect to the peripheral.
            os_log("Connecting to perhiperal %@", peripheral)
            centralManager.connect(peripheral, options: nil)
        }
    }
    
    /*
     *  We've connected to the peripheral, now we need to discover the services and characteristics to find the 'transfer' characteristic.
     */
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        os_log("Peripheral Connected")
        
        // Stop scanning
        centralManager.stopScan()
        os_log("Scanning stopped")
        
        // set iteration info
        connectionIterationsComplete += 1
        writeIterationsComplete = 0
        
        // Clear the data that we may already have
        data.removeAll(keepingCapacity: false)
        
        // Make sure we get the discovery callbacks
        peripheral.delegate = self

        // Search only for services that match our UUID
        peripheral.discoverServices([flightControllerServiceUUID])

    }
    
    /*
     *  If the connection fails for whatever reason, we need to deal with it.
     */
    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
        os_log("Failed to connect to %@. %s", peripheral, String(describing: error))
        cleanup()
    }
    
    /*
     *  Once the disconnection happens, we need to clean up our local copy of the peripheral
     */
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        os_log("Perhiperal Disconnected")
        os_log("Error: %s",  String(describing: error))
        discoveredPeripheral = nil
        
        // We're disconnected, so start scanning again
        if connectionIterationsComplete < defaultIterations {
            retrievePeripheral()
        } else {
            os_log("Connection iterations completed")
        }
    }
}
        
extension BluetoothViewModel: CBPeripheralDelegate {
    // implementations of the CBPeripheralDelegate methods
    
    /*
     *  The peripheral letting us know when services have been invalidated.
     */
    func peripheral(_ peripheral: CBPeripheral, didModifyServices invalidatedServices: [CBService]) {
        
        for service in invalidatedServices where service.uuid == flightControllerServiceUUID {
            os_log("Transfer service is invalidated - rediscover services")
            peripheral.discoverServices([flightControllerServiceUUID])
        }
    }
    
    /*
     *  The Transfer Service was discovered
     */
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        if let error = error {
            os_log("Error discovering services: %s", error.localizedDescription)
            cleanup()
            return
        }
        
        // Discover the characteristic we want...
        os_log("running peripheral didDiscoverServices")
        // Loop through the newly filled peripheral.services array, just in case there's more than one.
        guard let peripheralServices = peripheral.services else {
            os_log("Peripheral has no service")
            return }
        
        os_log("%d", peripheralServices.count)
        for service in peripheralServices {
            os_log("searching peripheral service's characteristics ")
            peripheral.discoverCharacteristics([flightControllerCharacteristicUUID], for: service)
        }
        os_log("Done searching peripheral service's characteristics")
    }
    
    /*
     *  The Transfer characteristic was discovered.
     *  Once this has been found, we want to subscribe to it, which lets the peripheral know we want the data it contains
     */
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        // Deal with errors (if any).
        if let error = error {
            os_log("Error discovering characteristics: %s", error.localizedDescription)
            cleanup()
            return
        }
        os_log("running peripheral didDiscoverCharacteristicsFor")
        // Again, we loop through the array, just in case and check if it's the right one
        guard let serviceCharacteristics = service.characteristics else { return }
        for characteristic in serviceCharacteristics where characteristic.uuid == flightControllerCharacteristicUUID {
            // If it is, subscribe to it
            transferCharacteristic = characteristic
            os_log("Subscribe to characteristic")
            peripheral.setNotifyValue(true, for: characteristic)
        }
        
//        writeData(0)
    }
    
}
    

