# hakoniwa-pdu-unreal

**hakoniwa-pdu-unreal** is an Unreal Engine plugin for **receiving and sending PDU (Protocol Data Unit)** data.
It enables **real-time synchronization** of position, orientation, and other state information via HTTP or WebSocket communication with an external Hakoniwa PDU WebServer.
This makes it possible to accurately visualize drones, robots, and other simulated entities inside Unreal Engine.

---

## ✨ Features

* Bidirectional communication with Hakoniwa PDU WebServer (receive & send)
* Real-time application of position, orientation, and other state data to UE Actors
* Modular plugin architecture for easy reuse across projects
* Easily extendable to support sensors, animations (e.g., propeller rotation), or AR integration

---

## 🧩 Architecture

![hakoniwa-unreal-architecture](https://github.com/user-attachments/assets/32b2504d-d83f-4668-89fb-44c2fdde2446)

This architecture separates **simulation** and **visualization** concerns:

* The simulation runs independently within the Hakoniwa Core system.
* Unreal Engine acts as a visualization client, reflecting real-world-like dynamics through received PDU data.

---
