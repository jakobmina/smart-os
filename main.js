import React, { useState, useEffect, useRef, useMemo } from 'react';
import { Settings, Info, Zap, Activity, ShieldCheck, Database } from 'lucide-react';

const App = () => {
  const [rotation, setRotation] = useState({ x: 0, y: 0 });
  const [isDragging, setIsDragging] = useState(false);
  const [lastMousePos, setLastMousePos] = useState({ x: 0, y: 0 });
  const [noiseLevel, setNoiseLevel] = useState(0.5);
  const [laminarControl, setLaminarControl] = useState(true);
  const [activeBit, setActiveBit] = useState(null);
  const [showLabels, setShowLabels] = useState(true);

  // Constants from QuoreMind Framework
  const N_BITS = 158; // 16 * pi^2
  const GOLDEN_RATIO = (1 + Math.sqrt(5)) / 2;

  // Generate 158 bits using Fibonacci sphere algorithm for uniform distribution
  const bits = useMemo(() => {
    const tempBits = [];
    for (let i = 0; i < N_BITS; i++) {
      const y = 1 - (i / (N_BITS - 1)) * 2; // y goes from 1 to -1
      const radius = Math.sqrt(1 - y * y); // radius at y

      const theta = 2 * Math.PI * GOLDEN_RATIO * i; // Golden angle increment

      const x = Math.cos(theta) * radius;
      const z = Math.sin(theta) * radius;

      // Operator On = cos(pi * n) * cos(pi * phi * n)
      const operatorVal = Math.cos(Math.PI * i) * Math.cos(Math.PI * GOLDEN_RATIO * i);
      const isBosonic = i % 2 === 0;

      tempBits.push({
        id: i,
        x, y, z,
        operatorVal,
        isBosonic,
        phase: Math.random() * Math.PI * 2
      });
    }
    return tempBits;
  }, []);

  const handleMouseDown = (e) => {
    setIsDragging(true);
    setLastMousePos({ x: e.clientX, y: e.clientY });
  };

  const handleMouseMove = (e) => {
    if (!isDragging) return;
    const dx = e.clientX - lastMousePos.x;
    const dy = e.clientY - lastMousePos.y;
    setRotation(prev => ({
      x: prev.x + dy * 0.5,
      y: prev.y + dx * 0.5
    }));
    setLastMousePos({ x: e.clientX, y: e.clientY });
  };

  const handleMouseUp = () => setIsDragging(false);

  // Auto-rotation when not dragging
  useEffect(() => {
    const timer = setInterval(() => {
      if (!isDragging) {
        setRotation(prev => ({ ...prev, y: prev.y + 0.2 }));
      }
    }, 30);
    return () => clearInterval(timer);
  }, [isDragging]);

  return (
    <div className="flex flex-col h-screen bg-slate-950 text-slate-200 overflow-hidden select-none font-sans"
      onMouseMove={handleMouseMove}
      onMouseUp={handleMouseUp}
      onMouseLeave={handleMouseUp}>

      {/* Header */}
      <header className="p-4 border-b border-slate-800 bg-slate-900/50 flex justify-between items-center backdrop-blur-md z-10">
        <div className="flex items-center gap-3">
          <div className="p-2 bg-blue-600 rounded-lg">
            <Activity size={20} className="text-white" />
          </div>
          <div>
            <h1 className="font-bold text-lg leading-tight">QuoreMind OS Explorer</h1>
            <p className="text-xs text-slate-400">Holographic Byte Visualizer v1.0</p>
          </div>
        </div>
        <div className="flex gap-4">
          <div className="flex flex-col items-end">
            <span className="text-[10px] text-slate-500 uppercase tracking-widest">Quantum State</span>
            <span className="text-sm font-mono text-emerald-400">LAMINAR_COHERENT</span>
          </div>
        </div>
      </header>

      <div className="flex flex-1 relative">
        {/* Sidebar Controls */}
        <aside className="w-72 bg-slate-900/80 border-r border-slate-800 p-6 flex flex-col gap-6 z-10 overflow-y-auto">
          <section>
            <h2 className="text-xs font-semibold text-slate-500 uppercase tracking-wider mb-4 flex items-center gap-2">
              <Settings size={14} /> Metriplectic Controls
            </h2>
            <div className="space-y-4">
              <div>
                <label className="text-sm block mb-2">Noise Pressure (PRN)</label>
                <input
                  type="range"
                  min="0" max="100"
                  value={noiseLevel * 100}
                  onChange={(e) => setNoiseLevel(e.target.value / 100)}
                  className="w-full accent-blue-500"
                />
              </div>
              <div className="flex items-center justify-between p-3 bg-slate-800/50 rounded-lg border border-slate-700">
                <span className="text-sm">Laminar Control</span>
                <button
                  onClick={() => setLaminarControl(!laminarControl)}
                  className={`w-12 h-6 rounded-full transition-colors relative ${laminarControl ? 'bg-blue-600' : 'bg-slate-600'}`}
                >
                  <div className={`absolute top-1 w-4 h-4 bg-white rounded-full transition-transform ${laminarControl ? 'left-7' : 'left-1'}`} />
                </button>
              </div>
            </div>
          </section>

          <section className="bg-blue-900/20 border border-blue-800/50 rounded-xl p-4">
            <h3 className="text-sm font-bold text-blue-400 mb-2 flex items-center gap-2">
              <ShieldCheck size={16} /> SC-QEC Active
            </h3>
            <p className="text-xs text-slate-300 leading-relaxed">
              Applying Sign Law Projection: 158 degrees of freedom stabilized by Golden Quasiperiodic Operator.
            </p>
          </section>

          <section>
            <h2 className="text-xs font-semibold text-slate-500 uppercase tracking-wider mb-4 flex items-center gap-2">
              <Info size={14} /> Bit Data
            </h2>
            {activeBit !== null ? (
              <div className="p-3 bg-slate-800 rounded-lg border border-blue-500/30 animate-pulse">
                <p className="text-xs text-blue-400 font-mono">BIT_ID: {activeBit.id}</p>
                <p className="text-sm mt-1">Status: {activeBit.isBosonic ? 'BOSONIC_SURVIVAL' : 'FERMIONIC_NULL'}</p>
                <p className="text-xs text-slate-400 mt-2">On: {activeBit.operatorVal.toFixed(4)}</p>
              </div>
            ) : (
              <p className="text-xs text-slate-500 italic">Hover a pixel to inspect holographic data.</p>
            )}
          </section>
        </aside>

        {/* Main Viewport */}
        <main className="flex-1 bg-black flex items-center justify-center relative perspective-1000 cursor-grab active:cursor-grabbing overflow-hidden"
          onMouseDown={handleMouseDown}>

          {/* Background Grid/Stars */}
          <div className="absolute inset-0 opacity-10 pointer-events-none"
            style={{ backgroundImage: 'radial-gradient(circle, #3b82f6 1px, transparent 1px)', backgroundSize: '40px 40px' }} />

          {/* Labels */}
          {showLabels && (
            <>
              <div className="absolute top-10 left-10 text-slate-600 font-mono text-xs border-l border-slate-700 pl-4">
                VACUUM_IMPEDANCE: α ≈ 1/137.036<br />
                GEOMETRIC_FRICTION: 7.6152<br />
                PHI: 1.618033
              </div>
              <div className="absolute bottom-10 right-10 text-slate-600 font-mono text-xs border-r border-slate-700 pr-4 text-right">
                REYNOLDS_INFORMATIONAL: 1.000<br />
                BIHARI_LIMIT: 0.852<br />
                CAUSAL_FRAGMENTS: 19
              </div>
            </>
          )}

          {/* The Sphere */}
          <div className="relative w-96 h-96 transition-transform duration-100 ease-out"
            style={{
              transform: `rotateX(${rotation.x}deg) rotateY(${rotation.y}deg)`,
              transformStyle: 'preserve-3d'
            }}>

            {/* Core Glow */}
            <div className="absolute inset-0 rounded-full blur-3xl bg-blue-600/10 pointer-events-none" />

            {/* Bits */}
            {bits.map((bit) => {
              const r = 180;
              const x = bit.x * r;
              const y = bit.y * r;
              const z = bit.z * r;

              const jitter = !laminarControl ? Math.random() * noiseLevel * 20 : 0;
              const glow = bit.isBosonic ? 'bg-blue-400 shadow-[0_0_8px_rgba(96,165,250,0.8)]' : 'bg-amber-400 opacity-50';

              return (
                <div
                  key={bit.id}
                  className={`absolute w-1.5 h-1.5 rounded-full transition-all duration-300 hover:scale-150 cursor-crosshair ${glow}`}
                  onMouseEnter={() => setActiveBit(bit)}
                  style={{
                    transform: `translate3d(${x + 192 + jitter}px, ${y + 192 + jitter}px, ${z}px)`,
                    opacity: 0.3 + (z + r) / (2 * r) * 0.7
                  }}
                />
              );
            })}

            {/* Orbitals */}
            <div className="absolute inset-0 border border-blue-500/20 rounded-full pointer-events-none" style={{ transform: 'rotateX(90deg)' }} />
            <div className="absolute inset-0 border border-blue-500/20 rounded-full pointer-events-none" style={{ transform: 'rotateY(90deg)' }} />
          </div>
        </main>

        {/* Legend */}
        <div className="absolute bottom-6 left-80 flex gap-6 z-10">
          <div className="flex items-center gap-2">
            <div className="w-3 h-3 rounded-full bg-blue-400 shadow-[0_0_5px_#60a5fa]" />
            <span className="text-[10px] text-slate-400 font-mono">BOSONIC (LAMINAR)</span>
          </div>
          <div className="flex items-center gap-2">
            <div className="w-3 h-3 rounded-full bg-amber-400 opacity-50" />
            <span className="text-[10px] text-slate-400 font-mono">FERMIONIC (NOISE)</span>
          </div>
        </div>
      </div>

      {/* Footer Metrics */}
      <footer className="h-10 bg-slate-900 border-t border-slate-800 flex items-center px-4 justify-between font-mono text-[10px]">
        <div className="flex gap-6">
          <span className="flex items-center gap-1 text-slate-400">
            <Database size={12} /> HOLOGRAPHIC_CAPACITY: 157.91 BITS
          </span>
          <span className="flex items-center gap-1 text-slate-400">
            <Zap size={12} /> ZEEMAN_SENSITIVITY: 94.2%
          </span>
        </div>
        <div className="flex gap-4">
          <span className="text-blue-500">SYS_OK</span>
          <span className="text-slate-600">FRAG_ID: QUORE-001-TLX</span>
        </div>
      </footer>
    </div>
  );
};

export default App;