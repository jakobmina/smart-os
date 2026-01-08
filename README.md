# Smart-OS: The Photonic-Metriplectic Kernel

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Stability: Stable](https://img.shields.io/badge/Stability-Stable-blue.svg)]()
[![Target: i386-Baremetal](https://img.shields.io/badge/Target-i386--Baremetal-orange.svg)]()

**Smart-OS** is a specialized baremetal operating system kernel designed for **Photonic Information Transport** using the principles of **Metriplectic Dynamics** and **Topological Lindblad Pumping**.

## 🌌 Core Philosophy: El Mandato Metriplético

The kernel's physics core operates on two orthogonal brackets:

1. **Hamiltonian Component ({u, H})**: Conservative, reversible dynamics (Z-Pinch oscillations).
2. **Metric Component ([u, S])**: Dissipative, irreversible relaxation (Laminar stabilization).

---

## 🎯 Purpose & Audience

The **QuoreMind Q-CORE** is not a conventional OS. It is a physical-informational engine designed to bridge the gap between classical silicon and quantum topology.

### Target Audience

* **High-Resiliency System Architects:** Building infrastructures that must self-stabilize under extreme data loads or noise.
* **Hybrid Quantum-Classical Researchers:** Bridging bits and quantum fields using physical analogies (Z-Pinch, Toroidal Resonance).
* **Information Theorists (DIT):** Exploring "Data as a Fluid" where information has viscosity, inertia, and entropy.

### Problems Solved

* **Chaotic Instability (Jitter):** Solves data turbulence using the **Shear Flow Canal**. Much like a fusion reactor, it stabilizes information flow inside a virtual magnetic channel.
* **Core Desynchronization (Entropy):** Solves inter-core latency using **Toroidal Resonance & "Breathing"**. The system "pumps" data in synchronous laser-like windows, ensuring all cores share a "Global Identity".
* **System Decay/Explosion:** Prevents "thermal death" or numerical blow-ups by maintaining **Metriplectic Homeostasis**—a constant competition between conservation and dissipation.

---

## ⚡ Photonic OS Features

### 1. Baremetal C Kernel

- **Freestanding Environment**: Zero dependencies on host OS or standard libraries.
* **Multiboot1 Compliant**: Boots directly in QEMU or on physical i386 hardware.
* **VGA Text-Art Rendering**: High-fidelity 16-color ASCII visualization of plasma columns at `0xB8000`.

### 2. Lindblad Master Engine

- **Photonic Transport**: Models laser-pumped information nodes acting as quantum excitations in a plasma substrate.
* **Decoherence Coupling**: Information loss is mathematically coupled to plasma instability—only a **Shear Mach 10** state maintains data coherence.

### 3. Causal Photonic Reduction

- **Golden Ratio Filter (Φ)**: Implements the recursive causal reduction:
  $$\Phi_f = \frac{\Phi \cdot \Phi_f + 1}{2}$$
* **Packet Fragmentation**: Reduces **5V** information packets into stable binary states through the "/2" final constraint of real binary physics.

---

## 🚀 Getting Started

### Prerequisites

- `gcc` (with 32-bit support)
* `nasm`
* `make`
* `qemu-system-i386`

### Building and Running

```bash
cd kernel
make -f Makefile.qemu run
```

### Monitoring (Serial Port)

The kernel outputs a dynamic DIT-Throughput heartbeat to the COM1 serial port (`0x3F8`). To view logs in the terminal:

```bash
qemu-system-i386 -kernel kernel.bin -serial stdio
```

---

## 🛠 Project Structure

* `kernel/`: The core Baremetal OS source (C, x86 ASM, Linker scripts).
* `z-pinch-ui/`: Legacy React dashboard for high-level simulation monitoring.
* `kernel/qcore_metriplectic.c`: The physical engine implementing Lindblad & Metriplectic solvers.

---

## 📜 Credits & License

**Developed by**: Jacobo Tlacaelel Mina Rodriguez
**Architecture**: QuoreMind - Topological Centroid Governance
**Year**: 2026

License: MIT
