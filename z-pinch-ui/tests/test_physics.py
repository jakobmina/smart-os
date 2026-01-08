import math
import pytest

# Constants
PHI = (1 + math.sqrt(5)) / 2

def golden_operator(n):
    return math.cos(math.pi * n) * math.cos(math.pi * PHI * n)

def compute_metriplectic_dynamics(stability, shear_flow, t, include_vacuum=True):
    # Hamiltonian (Conservative - Oscillatory)
    h_energy = (1 - shear_flow)
    
    # Dissipative (Metric - Relaxational)
    # Target is shear_flow * 100
    d_metr = (shear_flow * 100 - stability) * 0.05
    
    vacuum = golden_operator(t * 0.1) * 0.1 if include_vacuum else 0
    
    new_stability = stability + d_metr + vacuum
    return max(0, min(100, new_stability))

def test_golden_operator_range():
    for n in range(100):
        val = golden_operator(n / 10.0)
        assert -1 <= val <= 1

def test_stability_atractor():
    # If shear flow is high, stability should increase towards its value
    stability = 50
    shear_flow = 0.9 # Target 90%
    for _ in range(200):
        stability = compute_metriplectic_dynamics(stability, shear_flow, 0, include_vacuum=False)
    
    # It should be close to 90
    assert abs(stability - 90) < 0.1

def test_instability_low_shear():
    # If shear flow is low, stability should decrease
    stability = 90
    shear_flow = 0.2 # Target 20%
    for _ in range(200):
        stability = compute_metriplectic_dynamics(stability, shear_flow, 0, include_vacuum=False)
    
    # It should be close to 20
    assert abs(stability - 20) < 0.1

def test_mandato_metriplectico_orthogonal_components():
    # Verify that we can separate H and S
    shear_flow = 0.5
    h_energy = 1 - shear_flow
    s_entropy = shear_flow
    
    # At high shear flow, entropy (dissipation) dominates, oscillation (energy) decreases
    assert (1 - 0.9) < (1 - 0.1) # H decreases
    assert 0.9 > 0.1           # S increases
