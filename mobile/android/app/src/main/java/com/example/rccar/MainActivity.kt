package com.example.rccar

import android.app.Activity
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.os.Bundle
import android.view.View
import android.widget.*
import java.io.IOException
import java.io.OutputStream
import java.util.UUID

class MainActivity : Activity() {

    private val btAdapter: BluetoothAdapter? = BluetoothAdapter.getDefaultAdapter()
    private lateinit var listView: ListView
    private var deviceList = ArrayList<BluetoothDevice>()
    private var socket: BluetoothSocket? = null
    private var outStream: OutputStream? = null

    // Standard SPP UUID
    private val SPP_UUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        listView = findViewById(R.id.list_devices)
        val btnScan: Button = findViewById(R.id.btn_scan)
        btnScan.setOnClickListener { scanPairedDevices() }

        val btnF: Button = findViewById(R.id.btn_forward)
        val btnL: Button = findViewById(R.id.btn_left)
        val btnS: Button = findViewById(R.id.btn_stop)
        val btnR: Button = findViewById(R.id.btn_right)
        val btnB: Button = findViewById(R.id.btn_back)

        btnF.setOnClickListener { sendCmd("F200\n") }
        btnB.setOnClickListener { sendCmd("B200\n") }
        btnL.setOnClickListener { sendCmd("L180\n") }
        btnR.setOnClickListener { sendCmd("R180\n") }
        btnS.setOnClickListener { sendCmd("S\n") }

        listView.setOnItemClickListener { _, _, position, _ ->
            val device = deviceList[position]
            connectToDevice(device)
        }
    }

    private fun scanPairedDevices() {
        deviceList.clear()
        val paired = btAdapter?.bondedDevices
        val names = ArrayList<String>()
        if (paired != null) {
            for (d in paired) {
                deviceList.add(d)
                names.add(d.name + "\n" + d.address)
            }
        }
        val adapter = ArrayAdapter(this, android.R.layout.simple_list_item_1, names)
        listView.adapter = adapter
    }

    private fun connectToDevice(device: BluetoothDevice) {
        // close previous
        try { socket?.close() } catch (e: Exception) {}
        Thread {
            try {
                socket = device.createRfcommSocketToServiceRecord(SPP_UUID)
                btAdapter?.cancelDiscovery()
                socket?.connect()
                outStream = socket?.outputStream
                runOnUiThread { Toast.makeText(this, "Connected to ${device.name}", Toast.LENGTH_SHORT).show() }
            } catch (e: IOException) {
                e.printStackTrace()
                runOnUiThread { Toast.makeText(this, "Connection failed", Toast.LENGTH_SHORT).show() }
                try { socket?.close() } catch (ex: Exception) {}
            }
        }.start()
    }

    private fun sendCmd(cmd: String) {
        if (outStream == null) {
            Toast.makeText(this, "Not connected", Toast.LENGTH_SHORT).show()
            return
        }
        Thread {
            try {
                outStream?.write(cmd.toByteArray())
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }.start()
    }

    override fun onDestroy() {
        try { socket?.close() } catch (e: Exception) {}
        super.onDestroy()
    }
}
