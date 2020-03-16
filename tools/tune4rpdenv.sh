#!/bin/bash

#performance governor
echo "trying to set CPU governor policy to performance"

if [[ -e "/sys/devices/system/cpu/cpufreq/policy0/scaling_governor" ]]; then
    echo "performance" | sudo tee /sys/devices/system/cpu/cpufreq/policy0/scaling_governor || echo " error, failed to set cpu govrnor policy "
else
    echo "unable to set CPU governor policy"
fi


echo "tring to disable turbo boost"

if [[ -e "/sys/devices/system/cpu/intel_pstate/no_turbo" ]]; then
    echo 1 | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo
else 
    echo "unable to disactivate turbo boost"
fi

