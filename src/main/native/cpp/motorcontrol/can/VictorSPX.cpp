#include "ctre/phoenix/motorcontrol/can/VictorSPX.h"

using namespace ctre::phoenix;
using namespace ctre::phoenix::motorcontrol::can;

//Construct defaults for utils
VictorSPXConfiguration VictorConfigUtil::_default;
VictorSPXPIDSetConfiguration VictorSPXPIDSetConfigUtil::_default;

/**
 * Constructor
 * @param deviceNumber [0,62]
 */
VictorSPX::VictorSPX(int deviceNumber) :
    BaseMotorController(deviceNumber | 0x01040000) {}


/**
 * Gets all PID set persistant settings.
 *
 * @param pid               Object with all of the PID set persistant settings
 * @param pidIdx            0 for Primary closed-loop. 1 for auxiliary closed-loop.
 * @param timeoutMs
 *              Timeout value in ms. If nonzero, function will wait for
 *              config success and report an error if it times out.
 *              If zero, no blocking or checking is performed.
 * @param enableOptimizations Enable the optimization technique
 */
ctre::phoenix::ErrorCode VictorSPX::ConfigurePID(const VictorSPXPIDSetConfiguration &pid, int pidIdx, int timeoutMs, bool enableOptimizations) {
    ErrorCollection errorCollection;
    
    //------ sensor selection ----------//      

    if(VictorSPXPIDSetConfigUtil::SelectedFeedbackCoefficientDifferent(pid) || !enableOptimizations)
		errorCollection.NewError(ConfigSelectedFeedbackCoefficient(pid.selectedFeedbackCoefficient, pidIdx, timeoutMs));
	
    /* This is ignored because Victor's firmware default value is impossible to set in API */
	//if(VictorSPXPIDSetConfigUtil::SelectedFeedbackSensorDifferent(pid) || !enableOptimizations)
	errorCollection.NewError(ConfigSelectedFeedbackSensor(pid.selectedFeedbackSensor, pidIdx, timeoutMs));        

	return errorCollection._worstError;
}
/**
 * Gets all PID set persistant settings (overloaded so timeoutMs is 50 ms
 * and pidIdx is 0).
 *
 * @param pid               Object with all of the PID set persistant settings
 */
void VictorSPX::GetPIDConfigs(VictorSPXPIDSetConfiguration &pid, int pidIdx, int timeoutMs)
{
	BaseGetPIDConfigs(pid, pidIdx, timeoutMs);
	pid.selectedFeedbackSensor = (RemoteFeedbackDevice) (int)ConfigGetParameter(eFeedbackSensorType, pidIdx, timeoutMs);

}


/**
 * Configures all peristant settings.
 *
 * @param allConfigs        Object with all of the persistant settings
 * @param timeoutMs
 *              Timeout value in ms. If nonzero, function will wait for
 *              config success and report an error if it times out.
 *              If zero, no blocking or checking is performed.
 *
 * @return Error Code generated by function. 0 indicates no error. 
 */
ErrorCode VictorSPX::ConfigAllSettings(const VictorSPXConfiguration &allConfigs, int timeoutMs) {
    ErrorCollection errorCollection;
	
	errorCollection.NewError(BaseConfigAllSettings(allConfigs, timeoutMs));	        
		
	//--------PIDs---------------//
	
    errorCollection.NewError(ConfigurePID(allConfigs.primaryPID, 0, timeoutMs, allConfigs.enableOptimizations));
        
    errorCollection.NewError(ConfigurePID(allConfigs.auxiliaryPID, 1, timeoutMs, allConfigs.enableOptimizations));
	
	if(VictorConfigUtil::ForwardLimitSwitchSourceDifferent(allConfigs)) 
		errorCollection.NewError(ConfigForwardLimitSwitchSource(allConfigs.forwardLimitSwitchSource, allConfigs.forwardLimitSwitchNormal, 
			allConfigs.forwardLimitSwitchDeviceID, timeoutMs));
	if(VictorConfigUtil::ReverseLimitSwitchSourceDifferent(allConfigs)) 
		errorCollection.NewError(ConfigReverseLimitSwitchSource(allConfigs.reverseLimitSwitchSource, allConfigs.reverseLimitSwitchNormal, 
			allConfigs.reverseLimitSwitchDeviceID, timeoutMs));
			
	if(VictorConfigUtil::Sum0TermDifferent(allConfigs)) errorCollection.NewError(ConfigSensorTerm(SensorTerm::SensorTerm_Sum0, allConfigs.sum0Term , timeoutMs));
	if(VictorConfigUtil::Sum1TermDifferent(allConfigs)) errorCollection.NewError(ConfigSensorTerm(SensorTerm::SensorTerm_Sum1, allConfigs.sum1Term , timeoutMs));
	if(VictorConfigUtil::Diff0TermDifferent(allConfigs)) errorCollection.NewError(ConfigSensorTerm(SensorTerm::SensorTerm_Diff0, allConfigs.diff0Term, timeoutMs));
	if(VictorConfigUtil::Diff1TermDifferent(allConfigs)) errorCollection.NewError(ConfigSensorTerm(SensorTerm::SensorTerm_Diff1, allConfigs.diff1Term, timeoutMs));
	
    return errorCollection._worstError;
}
/**
 * Gets all persistant settings.
 *
 * @param allConfigs        Object with all of the persistant settings
 * @param timeoutMs
 *              Timeout value in ms. If nonzero, function will wait for
 *              config success and report an error if it times out.
 *              If zero, no blocking or checking is performed.
 */
void VictorSPX::GetAllConfigs(VictorSPXConfiguration &allConfigs, int timeoutMs) {
	
	BaseGetAllConfigs(allConfigs, timeoutMs);
	
	GetPIDConfigs(allConfigs.primaryPID, 0, timeoutMs);
	GetPIDConfigs(allConfigs.auxiliaryPID, 1, timeoutMs);
    allConfigs.sum0Term = (RemoteFeedbackDevice)(int) ConfigGetParameter(eSensorTerm, 0, timeoutMs);
    allConfigs.sum1Term = (RemoteFeedbackDevice)(int) ConfigGetParameter(eSensorTerm, 1, timeoutMs);
    allConfigs.diff0Term = (RemoteFeedbackDevice)(int) ConfigGetParameter(eSensorTerm, 2, timeoutMs);
    allConfigs.diff1Term = (RemoteFeedbackDevice)(int) ConfigGetParameter(eSensorTerm, 3, timeoutMs);

	allConfigs.forwardLimitSwitchSource = (RemoteLimitSwitchSource)(int) ConfigGetParameter(eLimitSwitchSource, 0, timeoutMs);
	allConfigs.reverseLimitSwitchSource = (RemoteLimitSwitchSource)(int) ConfigGetParameter(eLimitSwitchSource, 1, timeoutMs);
	allConfigs.forwardLimitSwitchDeviceID = (int) ConfigGetParameter(eLimitSwitchRemoteDevID, 0, timeoutMs);
	allConfigs.reverseLimitSwitchDeviceID = (int) ConfigGetParameter(eLimitSwitchRemoteDevID, 1, timeoutMs);
	allConfigs.forwardLimitSwitchNormal = (LimitSwitchNormal)(int) ConfigGetParameter(eLimitSwitchNormClosedAndDis, 0, timeoutMs);
	allConfigs.reverseLimitSwitchNormal = (LimitSwitchNormal)(int) ConfigGetParameter(eLimitSwitchNormClosedAndDis, 1, timeoutMs);

}
