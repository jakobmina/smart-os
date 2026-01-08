import React, { useState, useEffect, useRef } from 'react';
import {
  Zap, Play, Pause, Activity, Shield, Info,
  TrendingUp, Download, Eye, UploadCloud,
  Skull, Search, ArrowRightLeft, ShieldAlert,
  Code, Binary, Radio, Terminal, FlaskConical, AlertTriangle
} from 'lucide-react';

const PHI = (1 + Math.sqrt(5)) / 2;
const O_n = (n) => Math.cos(Math.PI * n) * Math.cos(Math.PI * PHI * n);

const App = () => {
  // Logic States
  const [isSimulating, setIsSimulating] = useState(false);
  const [shearFlow, setShearFlow] = useState(5.0);
  const [stability, setStability] = useState(50);
  const [isStored, setIsStored] = useState(false);
  const [isTransmitting, setIsTransmitting] = useState(false);
  const [transitProgress, setTransitProgress] = useState(0);
  const [isTampered, setIsTampered] = useState(false);
  const [forensicLog, setForensicLog] = useState(null);
  const [biMoMsg, setBiMoMsg] = useState("");
  const [time, setTime] = useState(0);
  const [history, setHistory] = useState(Array(40).fill(50));

  const canvasRef = useRef(null);
  const timeRef = useRef(0);

  // Constraint Simulation Loop
  useEffect(() => {
    let animationFrame;
    if (isSimulating && !isTampered) {
      const step = () => {
        timeRef.current += 0.05;
        setTime(timeRef.current);

        setStability(prev => {
          const isAtLimit = shearFlow >= 9.9;
          const targetStability = isAtLimit ? 100 : (shearFlow * 8);
          const instabilityFactor = isAtLimit ? 0 : (10 - shearFlow) * 2;
          const noise = (Math.random() - 0.5) * instabilityFactor;

          const newStability = prev * 0.92 + targetStability * 0.08 + noise;
          return Math.max(0, Math.min(100, newStability));
        });

        if (Math.random() > 0.9) {
          setHistory(h => [...h.slice(1), stability]);
        }

        animationFrame = requestAnimationFrame(step);
      };
      animationFrame = requestAnimationFrame(step);
    }
    return () => cancelAnimationFrame(animationFrame);
  }, [isSimulating, shearFlow, isTampered, stability]);

  // BiMoType Transmission Logic
  useEffect(() => {
    let transitInterval;
    if (isTransmitting && !isTampered) {
      transitInterval = setInterval(() => {
        setTransitProgress(prev => {
          if (prev >= 100) {
            setIsTransmitting(false);
            return 100;
          }
          if (shearFlow < 9.5 && Math.random() > 0.92) {
            triggerTamper();
            return prev;
          }
          return prev + 2;
        });
      }, 50);
    }
    return () => clearInterval(transitInterval);
  }, [isTransmitting, isTampered, shearFlow]);

  // Canvas Drawing (v4.6 - Constraint Edition)
  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    const { width, height } = canvas;

    const draw = () => {
      ctx.clearRect(0, 0, width, height);
      const centerX = width / 2;
      const centerY = height / 2;
      const kinkAmplitude = (100 - stability) * 0.8;
      const isExtreme = shearFlow >= 9.9;

      // 1. Vacuum Mesh (O_n Modulated)
      const vacuumVal = O_n(time * 0.5);
      ctx.strokeStyle = isTampered ? 'rgba(244, 63, 94, 0.15)' : `rgba(59, 130, 246, ${0.05 + Math.abs(vacuumVal) * 0.1})`;
      ctx.lineWidth = 1;
      for (let i = 0; i < 12; i++) {
        ctx.beginPath();
        const rX = 60 + i * 22 + vacuumVal * 5;
        const rY = 160 + i * 5;
        ctx.ellipse(centerX, centerY, rX, rY, 0, 0, Math.PI * 2);
        ctx.stroke();
      }

      // 2. Plasma Column
      const segments = 160;
      const columnHeight = 380;
      const startY = centerY - 190;

      ctx.beginPath();
      const baseColor = isTampered ? '#f43f5e' : (isExtreme ? '#ffffff' : '#3b82f6');
      ctx.strokeStyle = baseColor;
      ctx.lineWidth = isTransmitting ? 32 : (isExtreme ? 6 : 12);
      ctx.lineCap = 'round';
      ctx.shadowBlur = isExtreme ? 50 : 20;
      ctx.shadowColor = isTampered ? '#f43f5e' : '#60a5fa';

      for (let i = 0; i <= segments; i++) {
        const y = startY + (i / segments) * columnHeight;
        const phiPhase = i * 0.08 * PHI;
        let xOffset = Math.sin(phiPhase + time * 1.5) * kinkAmplitude;

        if (isTampered) {
          xOffset += (Math.random() - 0.5) * 100;
        }

        const widthMod = isExtreme ? (Math.random() > 0.98 ? 15 : 0) : Math.cos(i * 0.4 + time * 3) * (10 - shearFlow) * 2;
        ctx.lineWidth = Math.max(2, ctx.lineWidth + widthMod);

        if (i === 0) ctx.moveTo(centerX + xOffset, y);
        else ctx.lineTo(centerX + xOffset, y);
      }
      ctx.stroke();
      ctx.shadowBlur = 0;

      // 3. Packet Transmission Particle
      if (isTransmitting && !isTampered) {
        const packetY = startY + (transitProgress / 100) * columnHeight;
        ctx.fillStyle = '#ffffff';
        ctx.shadowBlur = 40;
        ctx.shadowColor = '#fff';
        ctx.beginPath();
        ctx.arc(centerX, packetY, 18, 0, Math.PI * 2);
        ctx.fill();
        ctx.shadowBlur = 0;
      }
    };

    draw();
  }, [time, stability, isTampered, isTransmitting, transitProgress, shearFlow]);

  const runBiMoStore = () => {
    if (shearFlow < 9.9) return;
    setBiMoMsg(".-. ..-. ... -.-. / --.- --");
    setIsStored(true);
  };

  const triggerTamper = () => {
    setIsTampered(true);
    setStability(0);
    setIsTransmitting(false);
    setForensicLog({
      type_error: "BIMOTYPE_CONSTRAINT_BREACH",
      threshold_required: "10.0",
      found_at: shearFlow.toFixed(3),
      attacker_id: "INTERCEPT_0xAF_RES"
    });
  };

  const reset = () => {
    setIsTampered(false);
    setStability(100);
    setIsStored(false);
    setIsTransmitting(false);
    setBiMoMsg("");
    setForensicLog(null);
    timeRef.current = 0;
    setHistory(Array(40).fill(100));
  };

  return (
    <div className={`flex flex-col h-screen p-6 overflow-hidden bg-grid scanlines relative transition-colors duration-1000 ${isTampered ? 'bg-red-950/20' : 'bg-slate-950'}`}>

      {/* Header Section */}
      <header className={`flex justify-between items-center mb-6 relative z-10 glass-panel p-4 rounded-2xl ${isTampered ? 'breach-active' : ''}`}>
        <div className="flex items-center gap-4">
          <div className={`p-3 rounded-xl flicker transition-colors duration-500 ${isTampered ? 'bg-white text-rose-600' : 'bg-blue-600 text-white'}`}>
            {isTampered ? <Skull size={24} /> : <Shield size={24} />}
          </div>
          <div>
            <h1 className="text-xl font-bold tracking-tight text-white uppercase font-outfit">
              QuoreMind <span className={isTampered ? 'text-white' : 'text-blue-500'}>Constraint</span> v4.6
            </h1>
            <div className="flex items-center gap-2 mt-1">
              <span className={`w-2 h-2 rounded-full animate-pulse ${isTampered ? 'bg-white' : 'bg-emerald-500'}`}></span>
              <p className="text-[10px] text-slate-400 font-mono uppercase tracking-widest leading-none">
                {isTampered ? "SECURITY_BREACH_DETECTED" : `THRESHOLD_SYNC: ${shearFlow >= 9.9 ? "RESONANCE_ESTABLISHED" : "SEARCHING..."}`}
              </p>
            </div>
          </div>
        </div>

        <div className="flex items-center gap-6">
          <div className="flex gap-3">
            {isTransmitting && !isTampered && (
              <button onClick={triggerTamper} className="px-4 py-2 rounded-xl text-[10px] font-bold bg-rose-600/20 text-rose-400 border border-rose-500/30 hover:bg-rose-600 hover:text-white transition-all uppercase tracking-widest">
                Simulate Breach
              </button>
            )}
            <button
              onClick={isTampered ? reset : () => setIsSimulating(!isSimulating)}
              className={`px-8 py-3 rounded-xl flex items-center gap-3 transition-all duration-500 text-xs font-bold uppercase tracking-[0.2em] shadow-xl ${isTampered ? 'bg-white text-rose-600' : isSimulating ? 'bg-slate-800 text-slate-400 border border-slate-700' : 'bg-blue-600 text-white shadow-blue-600/20'}`}
            >
              {isTampered ? <RefreshCw size={16} /> : isSimulating ? <Pause size={16} fill="currentColor" /> : <Play size={16} fill="currentColor" />}
              {isTampered ? "Universal Reset" : isSimulating ? "System Pause" : "Initiate Control"}
            </button>
          </div>
        </div>
      </header>

      <main className="flex gap-6 flex-1 relative z-10">

        {/* Left Control Panel */}
        <section className="w-80 flex flex-col gap-6">
          <div className={`glass-panel p-6 rounded-3xl space-y-8 flex-1 ${isTampered ? 'border-rose-500/30 bg-rose-950/10' : ''}`}>
            <h2 className="text-[10px] font-bold text-slate-500 flex items-center gap-2 uppercase tracking-widest border-b border-slate-800 pb-4">
              <FlaskConical size={14} className={isTampered ? 'text-rose-400' : 'text-blue-500'} /> Symmetry Pressure
            </h2>

            <div className="space-y-6">
              <div className="flex justify-between items-center text-xs font-mono">
                <span className="text-slate-400">Shear Mach Intensity</span>
                <span className={`text-lg font-bold ${shearFlow >= 9.9 ? 'text-emerald-400' : 'text-blue-400'}`}>
                  {shearFlow.toFixed(2)} / 10.0
                </span>
              </div>
              <input
                type="range" min="0" max="10" step="0.01"
                value={shearFlow}
                onChange={(e) => setShearFlow(parseFloat(e.target.value))}
              />
              <p className="text-[9px] text-slate-500 italic leading-snug">
                Precision threshold 10.0 is the unique topological solution where entropy reaches the laminar attractor.
              </p>
            </div>

            <div className="space-y-4 pt-4 border-t border-slate-800/50">
              <div className="flex justify-between items-end">
                <span className="text-[10px] text-slate-500 font-mono uppercase">Coherence_Metric</span>
                <span className={`text-2xl font-bold font-mono ${isTampered ? 'text-rose-500' : 'text-white'}`}>{stability.toFixed(1)}%</span>
              </div>
              <div className="h-3 bg-slate-900 rounded-full overflow-hidden border border-slate-800 p-1">
                <div
                  className={`h-full rounded-full transition-all duration-300 ${isTampered ? 'bg-rose-500' : stability > 80 ? 'bg-emerald-500' : 'bg-blue-600'}`}
                  style={{ width: `${stability}%` }}
                />
              </div>
            </div>

            <div className="space-y-4 pt-6">
              <h3 className="text-[9px] font-bold text-slate-500 uppercase tracking-widest">BiMoType Buffer</h3>
              <div className={`p-4 rounded-2xl border font-mono text-center transition-all duration-500 ${isTampered ? 'bg-rose-500/10 border-rose-500/20' : 'bg-black/40 border-slate-800'}`}>
                <div className={`text-xl tracking-[0.4em] h-8 flex items-center justify-center ${isTampered ? 'text-rose-400' : 'text-blue-400'}`}>
                  {isTampered ? "ERR_COLLAPSE" : (biMoMsg || "0x00_IDLE")}
                </div>
              </div>
              {shearFlow < 9.9 && !isTampered && (
                <div className="text-[9px] text-amber-500 bg-amber-500/10 p-3 rounded-xl border border-amber-500/20 flex gap-3 items-start">
                  <AlertTriangle size={14} className="shrink-0" />
                  <span>Resonance 10.0 required for BiMoType encoding. Stability limited to 80%.</span>
                </div>
              )}
            </div>
          </div>

          <div className="flex flex-col gap-3">
            <button
              onClick={runBiMoStore}
              disabled={shearFlow < 9.9 || isStored || isTampered}
              className={`w-full py-4 text-[10px] font-bold rounded-2xl transition-all duration-300 uppercase tracking-widest border border-blue-500/20 glass-blue hover:bg-blue-600 hover:text-white disabled:opacity-30 disabled:hover:bg-transparent disabled:hover:text-inherit`}
            >
              1. Encode BiMoType (Req 10.0)
            </button>
            <button
              onClick={() => setIsTransmitting(true)}
              disabled={!isStored || isTransmitting || isTampered}
              className={`w-full py-4 text-[10px] font-bold rounded-2xl transition-all duration-300 uppercase tracking-widest border border-white/20 glass-panel hover:bg-white hover:text-black disabled:opacity-30 disabled:hover:bg-transparent disabled:hover:text-inherit`}
            >
              2. Project Signal Container
            </button>
          </div>
        </section>

        {/* Center: Observation Core */}
        <section className={`flex-1 glass-panel rounded-[40px] relative overflow-hidden group shadow-2xl ${isTampered ? 'border-rose-500/50 pulse-error' : 'border-white/5'}`}>
          <div className="decal-tl"></div>
          <div className="decal-br"></div>

          <canvas
            ref={canvasRef}
            width={800}
            height={600}
            className="w-full h-full cursor-crosshair"
          />

          {/* Internal HUD Overlays */}
          <div className="absolute top-8 left-8 p-5 glass-panel rounded-2xl border-white/5 backdrop-blur-3xl space-y-3 min-w-[180px]">
            <div className="flex justify-between items-center text-[10px] font-mono">
              <span className="text-slate-500 uppercase">Resonance</span>
              <span className={shearFlow >= 9.9 ? 'text-white' : 'text-blue-500'}>{(PHI * shearFlow).toFixed(4)}</span>
            </div>
            <div className="flex justify-between items-center text-[10px] font-mono">
              <span className="text-slate-500 uppercase">Phi_Mod</span>
              <span className="text-blue-400">{(O_n(time) * 100).toFixed(2)}%</span>
            </div>
            <div className="flex justify-between items-center text-[10px] font-mono">
              <span className="text-slate-500 uppercase">Vacuum</span>
              <span className={shearFlow >= 9.9 ? 'text-emerald-400' : 'text-slate-600'}>
                {shearFlow >= 9.9 ? "LOCKED" : "UNCALIBRATED"}
              </span>
            </div>
          </div>

          <div className="absolute bottom-8 right-8 p-6 glass-panel rounded-3xl border-white/10 text-center min-w-[220px]">
            <div className="text-[10px] text-slate-500 uppercase mb-2 font-mono tracking-[0.3em]">Causal History</div>
            <div className="flex items-end gap-[2px] h-12 justify-center">
              {history.map((val, i) => (
                <div
                  key={i}
                  className={`w-1 rounded-full transition-all duration-500 ${isTampered ? 'bg-rose-500/40' : val > 80 ? 'bg-emerald-500/40' : 'bg-blue-500/40'}`}
                  style={{ height: `${val}%` }}
                />
              ))}
            </div>
          </div>
        </section>

        {/* Right: Forensic Hub */}
        <section className="w-72 flex flex-col gap-6">
          <div className={`glass-panel p-6 rounded-3xl space-y-6 flex-1 ${isTampered ? 'border-rose-500/30' : ''}`}>
            <h2 className="text-[10px] font-bold text-slate-500 flex items-center gap-2 uppercase tracking-widest border-b border-slate-800 pb-4">
              <Search size={14} className={isTampered ? 'text-rose-400' : 'text-white'} /> Forensic Trace
            </h2>

            {isTampered && forensicLog ? (
              <div className="space-y-6 animate-in fade-in slide-in-from-bottom-2 duration-500">
                <div className="p-4 bg-rose-500/10 border border-rose-500/20 rounded-2xl space-y-3">
                  <div className="flex items-center gap-2 text-rose-500 font-bold text-[10px] uppercase">
                    <ShieldAlert size={14} /> Critical Error
                  </div>
                  <div className="text-[11px] font-mono text-white leading-relaxed">
                    {forensicLog.type_error}
                  </div>
                </div>

                <div className="space-y-4 font-mono text-[10px] border-l-2 border-rose-900/50 pl-4 py-1">
                  <div className="flex justify-between">
                    <span className="text-slate-500 uppercase italic">Threshold_Req</span>
                    <span className="text-white font-bold">{forensicLog.threshold_required}</span>
                  </div>
                  <div className="flex justify-between">
                    <span className="text-slate-500 uppercase italic">Measured_Mach</span>
                    <span className="text-rose-400 font-bold">{forensicLog.found_at}</span>
                  </div>
                  <div className="flex justify-between">
                    <span className="text-slate-500 uppercase italic">Attacker_ID</span>
                    <span className="text-rose-500 font-bold">{forensicLog.attacker_id}</span>
                  </div>
                </div>

                <div className="p-3 bg-black/40 rounded-xl border border-rose-950 text-[9px] text-rose-400 font-mono italic">
                  "System stability collapsed during projection due to sub-threshold shear velocity."
                </div>
              </div>
            ) : (
              <div className="h-full flex flex-col items-center justify-center space-y-4 opacity-50">
                <Radio size={40} className="text-slate-700 animate-pulse" />
                <div className="text-center space-y-1">
                  <div className="text-[10px] font-bold text-slate-500 uppercase tracking-widest">Awaiting Analysis</div>
                  <div className="text-[8px] text-slate-600 font-mono">CHANNEL_TRANSPARENT_0xFF</div>
                </div>
              </div>
            )}
          </div>

          <div className="glass-panel p-5 rounded-3xl h-44 border-white/5 relative overflow-hidden group">
            <div className="absolute inset-0 opacity-5 pointer-events-none group-hover:opacity-10 transition-all duration-700">
              <div className="w-full h-full bg-[radial-gradient(circle_at_center,var(--accent-blue)_0%,transparent_70%)]" />
            </div>
            <h3 className="text-[10px] font-bold text-slate-500 mb-4 flex items-center gap-2 uppercase tracking-widest">
              <Terminal size={14} className="text-blue-400" /> Log Trace
            </h3>
            <div className="font-mono text-[8px] text-slate-500 space-y-2">
              <div className="flex gap-2">
                <span className="text-blue-600 font-bold">[00:0.0]</span>
                <span>KERNEL_4.6_INIT: {isTampered ? "BREACH" : "OK"}</span>
              </div>
              <div className="flex gap-2">
                <span className="text-blue-600 font-bold">[05:3.1]</span>
                <span>DIT_PIPELINE: {shearFlow >= 9.9 ? "SYNCHRONIZED" : "STOCHASTIC"}</span>
              </div>
              <div className="flex gap-2">
                <span className="text-blue-600 font-bold">[12:8.4]</span>
                <span>RES_STATUS: {(PHI * shearFlow).toFixed(4)}</span>
              </div>
              {isTampered && (
                <div className="flex gap-2 animate-pulse">
                  <span className="text-rose-600 font-bold">[ERROR]</span>
                  <span className="text-rose-500">CONSTRAINT_VIOLATION_DETECTED</span>
                </div>
              )}
            </div>
          </div>
        </section>
      </main>

      <footer className="mt-6 flex justify-between items-center text-[9px] text-slate-600 font-mono tracking-widest relative z-10 p-2 border-t border-white/5">
        <div className="flex items-center gap-6">
          <span className="flex items-center gap-2">
            <Binary size={12} className="text-slate-800" />
            DIT_ENGINE_4.6
          </span>
          <span className="text-slate-800">|</span>
          <span className="flex items-center gap-2">
            <Activity size={12} className="text-slate-800" />
            STABLE_LAMINAR: {shearFlow >= 9.9 ? "ACTIVE" : "PENDING"}
          </span>
        </div>
        <div>(C) 2026 // JACOBO TLACAELEL MINA RODRIGUEZ // BIMOTYPE_CONSTRAINTS</div>
      </footer>
    </div>
  );
};

export default App;
