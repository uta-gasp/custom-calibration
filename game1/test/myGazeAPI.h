/**
*
* myGaze API Documentation
*
* The myGaze Software Development Kit ("SDK") provides an Application Interface ("API") for
* communication between your customized software application and the myGaze eyetracking-server,
* allowing you to create full-featured eye tracking applications that take advantage of the powerful
* features offered by myGaze eye tracking device. Specifically, the SDK was designed for add eye
* tracking technology into their own custom applications. Using the functions provided in the SDK
* you can control VI myGaze eye tracking device and retrieve eye tracking data. The SDK delivery
* a high level Interface which provides maximum speed and minimum latency for data transfer.
* Additionally, the SDK supports a growing number of programming languages and environments
* including, but not limited to, MATLAB, C/C++, C#, and Python. Several example programs
* are provided for helping you get started with your application development.
*
*
* (C) Copyright 2015, Visual Interaction GmbH
*
* All rights reserved. This work contains unpublished proprietary information of
* Visual Interaction GmbH and is copy protected by law. It may not be disclosed
* to third parties or copied or duplicated in any form, in whole or in part,
* without the specific written permission of Visual Interaction GmbH
*
* @author Visual Interaction GmbH
*
*/

/**
* @file myGazeAPI.h
*
* @brief The file contains the prototype declaration for all supported functions and data
* structs the customer needs to use to get access the myGaze eyetracking device.
**/

#pragma once

#ifndef CALLBACK
#	define CALLBACK __stdcall
#endif

//for ms vc++
#if defined(_MSC_VER)
#	define DEPRECATED __declspec(deprecated)
#	if defined(__cplusplus)
#		define DLL_DEC_PRE extern "C" __declspec(dllimport)
#		define DEPRECATED_DLL_DEC_PRE extern "C" __declspec(deprecated, dllimport)
#	else
#		define DLL_DEC_PRE extern __declspec(dllimport)
#		define DEPRECATED_DLL_DEC_PRE extern __declspec(deprecated, dllimport)
#	endif
#	define DLL_DEC_POST __stdcall
#else
// for g++
#	define DEPRECATED
#	define DLL_DEC_PRE extern "C" __stdcall
#	define DEPRECATED_DLL_DEC_PRE extern "C" __stdcall
#	define DLL_DEC_POST
#endif


#define RET_SUCCESS													1
#define RET_DATA_INVALID											2
#define RET_CALIBRATION_ABORTED										3
#define RET_SERVER_IS_RUNNING										4
#define RET_CALIBRATION_NOT_IN_PROGRESS								5
#define RET_WINDOW_IS_OPEN											11
#define RET_WINDOW_IS_CLOSED										12

#define ERR_CONNECTION_REFUSED										100
#define ERR_CONNECTION_NOT_ESTABLISHED								101
#define ERR_CALIBRATION_NOT_AVAILABLE								102
#define ERR_CALIBRATION_NOT_VALIDATED								103
#define ERR_SERVER_NOT_RUNNING										104
#define ERR_SERVER_NOT_RESPONDING									105
#define ERR_PARAMETER_INVALID										112
#define ERR_PARAMETER_CALIBRATION_INVALID							113
#define ERR_CALIBRATION_TIMEOUT										114
#define ERR_TRACKING_NOT_STABLE										115
#define ERR_SOCKET_CREATE											121
#define ERR_SOCKET_CONNECT											122
#define ERR_SOCKET_BIND												123
#define ERR_SOCKET_DELETE											124
#define ERR_SERVER_NO_RESPONSE										131
#define ERR_SERVER_VERSION_INVALID									132
#define ERR_SERVER_VERSION_UNKNOWN									133
#define ERR_FILE_ACCESS												171
#define ERR_SOCKET_ERROR											181
#define ERR_SERVER_NOT_READY										194
#define ERR_SERVER_NOT_FOUND										201
#define ERR_SERVER_PATH_NOT_FOUND									202
#define ERR_SERVER_ACCESS_DENIED									203
#define ERR_SERVER_ACCESS_INCOMPLETE								204
#define ERR_SERVER_OUT_OF_MEMORY									205
#define ERR_MULTIPLE_DEVICES										206
#define ERR_DEVICE_NOT_FOUND										211
#define ERR_DEVICE_UNKNOWN											212
#define ERR_DEVICE_CONNECTED_TO_WRONG_PORT							213
#define ERR_FEATURE_NOT_LICENSED									250
#define ERR_LICENSE_EXPIRED											251
#define ERR_DEPRECATED_FUNCTION										300


/** \addtogroup structs Data Types and Enumerations
 *  @{
 */

/**
* @enum CalibrationStatusEnum
*
* @brief This enum provides information about the eyetracking-server calibration status. If the
* device is not calibrated the eyetracking-server won't deliver valid gaze data. Use the functions
* @ref iV_GetCalibrationStatus to retrieve the calibration status and
* @ref iV_Calibrate to perform a calibration.
*/
enum CalibrationStatusEnum
{
	//! calibration status is unknown (i.e. if the connection is not established)
	unknownCalibrationStatus = 0,

	//! the device is not calibrated and will not deliver valid gaze data
	noCalibration = 1,

	//! the device is calibrated and will deliver valid gaze data
	validCalibration = 2,

	//! the device is currently performing a calibration
	performingCalibration = 3
};


/**
* @enum ETDevice
*
* @brief The enumeration ETDevice delivers information of the current connected device.
* @ref iV_GetSystemInfo to get information about which device is connected to eyetracking-server.
* It is part of the @ref SystemInfoStruct.
*/
enum ETDevice
{
	//! myGaze device
	myGaze = 2
};


/**
 * @enum GeometryType
 *
 * @brief used to the define the content of @ref GeometryStruct
 */
enum GeometryType
{
	//! default geometry type
	myGazeGeometry = 1
};

/*
* With ET_PARAM_EYE/ET_PARAM and function @ref iV_SetTrackingParameter it is possible
* to change myGaze tracking modes to use binocular or monocular tracking.
*/
#define ET_PARAM_EYE_LEFT				0
#define ET_PARAM_EYE_RIGHT				1
#define ET_PARAM_EYE_BOTH				2
#define ET_PARAM_MONOCULAR				16
#define ET_PARAM_SMARTBINOCULAR			17
#define ET_PARAM_BINOCULAR				18
#define ET_PARAM_SMARTMONOCULAR			19


/**
* @struct SystemInfoStruct
*
* @brief This struct provides information about the eyetracking-server version
* and the API version in use. To update data in @ref SystemInfoStruct use the function
* @ref iV_GetSystemInfo.
*/
struct SystemInfoStruct
{
	//! sample rate of eye tracking device in use
	int samplerate;

	//! major version number of eyetracking-server in use
	int iV_MajorVersion;

	//! minor version number of eyetracking-server in use
	int iV_MinorVersion;

	//! build number of eyetracking-server in use
	int iV_Buildnumber;

	//! major version number of myGaze API in use
	int API_MajorVersion;

	//! minor version number of myGaze API in use
	int API_MinorVersion;

	//! build number of myGaze API in use
	int API_Buildnumber;

	//! type of eye tracking device
	ETDevice iV_ETDevice;
};


/**
* @struct CalibrationPointStruct
*
* @brief This struct provides information about the position of calibration points. To
* update information in @ref CalibrationPointStruct use @ref iV_GetCalibrationPoint.
*/
struct CalibrationPointStruct
{
	//! number of calibration point
	int number;

	//! horizontal position of calibration point [pixel]
	int positionX;

	//! vertical position of calibration point [pixel]
	int positionY;
};

/**
 * @enum CalibrationPointUsageStatusEnum
 *
 * @brief This enum provides information about whether a calibration point was
 * actually used for the calibration or the reason why it was not used.
 * Use @ref iV_GetCalibrationQuality to retrieve the calibration quality data
 */
enum CalibrationPointUsageStatusEnum
{

	//! the calibration point was accepted and is used for the calibration
	calibrationPointUsed = 0,

	//! the calibration point was rejected and is not used for the calibration
	calibrationPointUnused = 1,

	//! the calibration point was rejected because no fixation was detected within reasonable time
	calibrationPointUnusedBecauseOfTimeout = 2,

	//! the calibration point was rejected because the detected fixation was to imprecise
	calibrationPointUnusedBecauseOfBadQuality = 3,

	//! no fixation was detected for the calibration point, but it is also not required
	calibrationPointIgnored = 4,
};

/**
 * @struct CalibrationPointQualityStruct
 *
 * @brief This struct provides information about the fixation quality when a
 * calibration point was shown. If the requested calibration quality
 * data is not available, the number and positionX/Y will be
 * set to -1. The user has to check these fields to make sure the
 * returned data is valid.
 *
 * @ref iV_GetCalibrationQuality to retrieve the calibration quality data
 */
struct CalibrationPointQualityStruct
{
	//! number of calibration point
	int number;

	//! horizontal position of calibration point [pixel]
	int positionX;

	//! vertical position of calibration point [pixel]
	int positionY;

	//! horizontal position of corrected fixation point [pixel]
	double correctedPorX;

	//! vertical position of corrected fixation point [pixel]
	double correctedPorY;

	//! horizontal standard deviation of the gaze samples, which represents
	//! the noise level of the fixation, given in [pixel]
	double standardDeviationX;

	//! vertical standard deviation of the gaze samples, which represents
	//! the noise level of the fixation, given in [pixel]
	double standardDeviationY;

	//! a flag indicating whether the calibration point was really used for
	//! the calibration or the reason why it was not used.
	CalibrationPointUsageStatusEnum usageStatus;

	//! quality index indicates how likely the user was really fixating on
	//! the calibration point when it was shown. It has a value between 0
	//! and 1. The smaller the value, the more likely the user was NOT
	//! fixating on the calibration point.
	double qualityIndex;
};

/**
* @struct EyeDataStruct
*
* @brief This struct provides numerical information about eye data.
* @ref EyeDataStruct is part of @ref SampleStruct. To update information
* in @ref SampleStruct use function @ref iV_GetSample or set the sample
* callback with @ref iV_SetSampleCallback.
*/
struct EyeDataStruct
{
	//! horizontal gaze position on screen [pixel]
	double gazeX;

	//! vertical gaze position on screen [pixel]
	double gazeY;

	//! pupil diameter [mm]
	double diam;

	//! horizontal eye position relative to camera [mm]
	double eyePositionX;

	//! vertical eye position relative to camera [mm]
	double eyePositionY;

	//! distance to camera [mm]
	double eyePositionZ;
};


/**
* @struct SampleStruct
*
* @brief This struct provides information about an eye data sample.
* To update information in @ref SampleStruct use the function
* @ref iV_GetSample or set the sample callback with @ref iV_SetSampleCallback.
* Note: Due to averaging binocular data the gaze values are similar for
* both eyes.
*/
struct SampleStruct
{
	//! timestamp of current gaze data sample [microseconds]
	__int64 timestamp;

	//! stores information of the left eye (see @ref EyeDataStruct for more information)
	EyeDataStruct leftEye;

	//! stores information of the right eye (see @ref EyeDataStruct for more information)
	EyeDataStruct rightEye;
};


/**
* @struct SampleStruct32
*
* @brief This struct provides information about a eye data samples.
* To update information in @ref SampleStruct32 use the function
* @ref iV_GetSample32. The difference to @ref SampleStruct is that
* the timestamp will be stored in milliseconds instead of microseconds.
* Note: Due to averaging binocular data the gaze values are similar for
* both eyes.
*/
struct SampleStruct32
{
	//! timestamp of current gaze data sample [milliseconds]
	double timestamp;

	//! stores information of the left eye (see @ref EyeDataStruct for more information)
	EyeDataStruct leftEye;

	//! stores information of the right eye (see @ref EyeDataStruct for more information)
	EyeDataStruct rightEye;
};


/**
* @struct EventStruct
*
* @brief This struct provides information about the last eye event that has
* been calculated.
* To update information in @ref EventStruct use function @ref iV_GetEvent or set
* the event callback with with @ref iV_SetEventCallback.
*/
struct EventStruct
{
	//! type of eye event, 'F' for fixation (only fixations are supported)
	char eventType;

	//! related eye, 'l' for left eye, 'r' for right eye
	char eye;

	//! start time of the event [microseconds]
	__int64 startTime;

	//! end time of the event [microseconds]
	__int64 endTime;

	//! duration of the event [microseconds]
	__int64 duration;

	//! horizontal position of the fixation event [pixel]
	double positionX;

	//! vertical position of the fixation event [pixel]
	double positionY;
};


/**
* @struct EventStruct32
*
* @brief This struct provides information about the last eye event that has
* been calculated.
* The difference to @ref EventStruct is that the timestamp will be stored in
* milliseconds instead of microseconds and the order of the components are different.
* To update information in @ref EventStruct32 use function @ref iV_GetEvent32.
*/
struct EventStruct32
{
	//! start time of the event [milliseconds]
	double startTime;

	//! end time of the event [milliseconds]
	double endTime;

	//! duration of the event [milliseconds]
	double duration;

	//! horizontal position of the fixation event [pixel]
	double positionX;

	//! vertical position of the fixation event [pixel]
	double positionY;

	//! type of eye event, 'F' for fixation (only fixations are supported)
	char eventType;

	//! related eye, 'l' for left eye, 'r' for right eye
	char eye;
};



/**
* @struct EyePositionStruct
*
* @brief This value represents the relative position of the eye in the tracking box.
* The 0 is defined at the center position. The value +1 defines the upper/right/far
* maximum while the value -1 the lower/left/near maximum.
* The position rating is related to the tracking monitor and represents how critical
* the tracking and the position is, related to the border of the tracking box. The 0
* is defined as the best eye position to be tracked while the value +1 defines that
* the eye is almost not being tracked due to extreme upper/right/far position. The
* value -1 defines that the eye is almost not being tracked due to extreme
* lower/left/near position.
* If the eye isn't tracked at all the validity flag goes to 0 and all values for the
* represented eye will be set to 0.
*/
struct EyePositionStruct
{
	//! confidence of position and rating values [0; 1]
	int validity;

	//! horizontal position [-1; +1]
	double relativePositionX;

	//! vertical position [-1; +1]
	double relativePositionY;

	//! depth/distance position [-1; +1]
	double relativePositionZ;

	//! horizontal rating [-1; +1]
	double positionRatingX;

	//! vertical rating [-1; +1]
	double positionRatingY;

	//! distance rating [-1; +1]
	double positionRatingZ;
};


/**
* @struct TrackingStatusStruct
*
* @brief This struct provides information about the relative eye ball position
* within the tracking box. The information will be provided for each eye individually
* as well as for the geographical center between both eyes. To update information in
* @ref TrackingStatusStruct use the function @ref iV_GetTrackingStatus.
*/
struct TrackingStatusStruct
{
	//! timestamp of current tracking status sample [microseconds]
	__int64 timestamp;

	//! stores information of the left eye (see EyePositionStruct for more information)
	EyePositionStruct leftEye;

	//! stores information of the right eye (see EyePositionStruct for more information)
	EyePositionStruct rightEye;

	//! stores information of the geometric average of both eyes (see EyePositionStruct for more information) 
	EyePositionStruct total;
};


/**
* @struct AccuracyStruct
*
* @brief This struct provides information about the last validation.
* Therefore a valid validation must be successfully completed before the AccuracyStruct can be
* updated. To update information in @ref AccuracyStruct use function @ref iV_GetAccuracy.
*/
struct AccuracyStruct
{
	//! horizontal calculated deviation for left eye [degree]
	double deviationLX;

	//! vertical calculated deviation for left eye [degree]
	double deviationLY;

	//! horizontal calculated deviation for right eye [degree]
	double deviationRX;

	//! vertical calculated deviation for right eye [degree]
	double deviationRY;
};

/**
* @struct GazeChannelQualityStruct
*
* @brief This struct provides information about the last validation.
* A validation must have been successfully completed before the
* GazeChannelQualityStruct can be updated. To update information in @ref
* GazeChannelQualityStruct use function @ref iV_GetGazeChannelQuality.
*/
struct GazeChannelQualityStruct
{
	//! Quality index of the left gaze channel. The value is in the range [0,1].
	//! A value > 0.5 means it is acceptable.
	//! A NaN value means it is not provided.
	double gazeChannelQualityLeft;

	//! Quality index of the right gaze channel. The value is in the range [0,1].
	//! A value > 0.5 means it is acceptable.
	//! A NaN value means it is not provided.
	double gazeChannelQualityRight;

	//! Quality index of the averaged gaze channel. The value is in the range [0,1].
	//! A value > 0.5 means it is acceptable.
	//! A NaN value means it is not provided.
	double gazeChannelQualityBinocular;
};


/**
* @struct CalibrationStruct
*
* @brief Use this struct to customize the calibration and validation behavior.
* To set calibration parameters with @ref CalibrationStruct use function
* @ref iV_SetupCalibration before a calibration or validation is started.
*/
struct CalibrationStruct
{

	//! select calibration method [# of points] (default: 5)
	//! A bit mask is used to specify a new calibration workflow. If
	//! the highest bit is 1, The "Smart Calibration" workflow
	//! should be activated on server.
	int method;

	//! draw calibration/validation by API [0: drawing disabled, 1: drawing enabled (default)]
	int visualization;

	//! set display device [0: primary device (default), 1: secondary device]
	int displayDevice;

	//! set calibration/validation speed [0: slow (default), 1: fast]
	int speed;

	//! set calibration/validation point acceptance [0: manual, 1: semi-automatic (default), 2: full-automatic]
	int autoAccept;

	//! set calibration/validation target brightness [0..255] (default: 250)
	int foregroundBrightness;

	//! set calibration/validation background brightness [0..255] (default: 220)
	int backgroundBrightness;

	//! set calibration/validation target shape [IMAGE = 0, CIRCLE1 = 1, CIRCLE2 = 2 (default), CROSS = 3]
	int targetShape;

	//! set calibration/validation target size [pixels] (default: 20)
	int targetSize;

	//! select custom calibration/validation target (only if targetShape = 0)
	char targetFilename[256];
};


/**
 * @struct GeometryStruct
 *
 * @brief Use this struct to customize the myGaze geometry.
 * See chapter @ref geometry in the myGaze SDK Manual for details.
 * For setting up the myGaze geometry parameters with
 * @ref GeometryStruct use function @ref iV_SetREDGeometry.
 */
struct GeometryStruct
{

	//! defines the geometry type, must be set to value: @ref myGazeGeometry
	GeometryType geometryType;

	//! this field is reserved and its value is ignored
	int reserved1;

	//! name of the profile
	char setupName [256];

	//! horizontal stimulus calibration size [mm]
	int stimX;

	//! vertical stimulus calibration size [mm]
	int stimY;

	//! this field is reserved and its value is ignored
	int reserved2 [3];

	//! myGaze inclination angle [degree]
	int inclAngle;

	//! vertical distance myGaze to stimulus screen [mm]
	int stimDistHeight;

	//! horizontal distance myGaze to stimulus screen [mm]
	int stimDistDepth;
};


/**
* @struct ImageStruct
*
* @brief Use this struct to get raw tracking monitor image (format: RGB 24bpp)
* in combination with @ref iV_GetTrackingMonitor, or set the tracking monitor
* callback with @ref iV_SetTrackingMonitorCallback.
*/
struct ImageStruct
{
	//! vertical size of the image [pixel]
	int imageHeight;

	//! horizontal size of the image [pixel]
	int imageWidth;

	//! image data size [byte]
	int imageSize;

	//! pointer to image data
	char* imageBuffer;
};


/**
* @struct DateStruct
*
* @brief Use this struct to get the license due date of the device. Use the
* function @ref iV_GetLicenseDueDate to update information in @ref DateStruct.
*/
struct DateStruct
{
	//! day of license expiration
	int day;

	//! month of license expiration
	int month;

	//! year of license expiration
	int year;
};


/**
* @struct MonitorAttachedGeometryStruct
*
* @brief Use this struct to customize myGaze geometry. (see chapter @ref geometry
* in the myGaze SDK Manual). For setting up the myGaze geometry parameters with
* MonitorAttachedGeometryStruct use function @ref iV_SetupMonitorAttachedGeometry.
*
* Warning! This structure is deprecated and will be removed in furture releases.
* Use @ref GeometryStruct instead.
*/
struct MonitorAttachedGeometryStruct{
	//! name of the setup
	char setupName[256];

	//! horizontal stimulus calibration size [mm]
	int stimX;

	//! vertical stimulus calibration size [mm]
	int stimY;

	//! vertical distance myGaze to stimulus screen [mm]
	int redStimDistHeight;

	//! horizontal distance myGaze to stimulus screen [mm]
	int redStimDistDepth;

	//! myGaze inclination angle [degree]
	int redInclAngle;
};



/**
* @brief sample callback function type
*/
typedef int (CALLBACK *pDLLSetSample)(SampleStruct sampleData);


/**
* @brief event callback function type
*/
typedef int (CALLBACK *pDLLSetEvent)(EventStruct eventData);


/**
* @brief tracking monitor image callback function type
*/
typedef int (CALLBACK *pDLLSetTrackingMonitor)(ImageStruct trackingMonitor);


/**
* @brief calibration callback function type
*/
typedef int (CALLBACK *pDLLSetCalibrationPoint)(CalibrationPointStruct calibrationPoint);

/** @}*/

/** \addtogroup functions Functions
 *  @{
 */

/**
 * @brief Aborts a calibration or validation if one is in progress. If the calibration or
 * validation function is visualizing the calibration area the @ref iV_Calibrate or
 * @ref iV_Validate function will return with RET_CALIBRATION_ABORTED.
 * See also @listcalibration
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_CALIBRATION_NOT_IN_PROGRESS			calibration or validation is not in progress 
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_AbortCalibration();

/**
 * @brief Abort waiting for fixation for a calibration or validation point when the calibration or
 * validation is in progress. If the latest calibration point has been accepted by the server,
 * the acceptance will be undone and the point unused. This allows an application to customize
 * the controlling logic of the calibration workflow, esp. when the calibration UI is
 * implemented by user.
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_CALIBRATION_NOT_IN_PROGRESS			calibration or validation is not in progress
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
**/
DLL_DEC_PRE int DLL_DEC_POST iV_AbortCalibrationPoint();


/**
 * @brief Accepts a calibration or validation point if the calibration or validation is in progress.
 * The participant needs to be tracked and has to fixate the calibration or validation point.
 * See also @listcalibration
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_CALIBRATION_NOT_IN_PROGRESS			calibration or validation is not in progress
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_AcceptCalibrationPoint();


/**
 * @brief Starts a calibration process. To proceed, the participant needs to be tracked and
 * has to fixate the calibration point. Depending on the calibration settings (which can
 * be changed using @ref iV_SetupCalibration) the user can accept the calibration points
 * manually (by pressing SPACE or calling @ref iV_AcceptCalibrationPoint) or abort the
 * calibration (by pressing ESC or calling @ref iV_AbortCalibration)<BR>
 * If the calibration is visualized by the API (CalibrationStruct::visualization is set to 1)
 * the function won't return until the calibration has been finished (closed automatically)
 * or aborted (ESC).<BR>
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_CALIBRATION_ABORTED					calibration was aborted during progress 
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_WRONG_CALIBRATION_METHOD			eye tracking device required for this
 *													calibration method is not connected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_Calibrate();


/**
 * @brief Changes the position of a calibration point. This has to be done before the calibration
 * process is started. The parameter number refers to the calibration method used.
 * See also @listcalibration
 *
 * @param	number									selected calibration point
 * @param	positionX								new X position on screen
 * @param	positionY								new Y position on screen
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_PARAMETER_INVALID					invalid parameter 
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ChangeCalibrationPoint(int number, int positionX, int positionY);


/**
 * @brief Establishes a connection to eyetracking-server. @ref iV_Connect will
 * not return until a connection has been established. If no connection can be
 * established, the function will return after the time span defined by
 * @ref iV_SetConnectionTimeout. Default time span is 3 seconds.
 * See also @listsystemstartstop
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_COULD_NOT_CONNECT					failed to establish connection
 * @retval	ERR_SOCKET_BIND							another process is blocking the communication
 * @retval	ERR_SERVER_NOT_RUNNING					eyetracking-server not running
 * @retval	ERR_SERVER_NOT_FOUND					eyetracking-server not detected
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_Connect();


/**
 * @brief Wakes up and enables the eye tracking application from suspend mode to
 * continue processing gaze data. The application can be set to suspend mode
 * by calling @ref iV_PauseEyetracking
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ContinueEyetracking();


/**
 * @brief Deletes the myGaze geometry profile with the given name. It is not possible
 * to delete a geometry profile if it is currently in use. See chapter
 * @ref geometry in the myGaze SDK Manual.
 *
 * @param	profileName								name of the geometry profile to delete
 *
 * @retval RET_SUCCESS								intended functionality has been fulfilled
 * @retval ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval ERR_PARAMETER_INVALID					parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_DeleteREDGeometry (char *profileName);


/**
 * @brief Disables a CPU high performance mode allowing the CPU to reduce the performance.
 * See also @ref iV_EnableProcessorHighPerformanceMode
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_DisableProcessorHighPerformanceMode();


/**
 * @brief Disables the eye tracker low power mode, which can results in
 * potentially higher power consumption but keeps the delay picking-up time short.
 * See also @ref iV_EnableLowPowerPickUpMode
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_IVIEWX_NOT_FOUND		The server is not responsive
 * @retval	ERR_WRONG_DEVICE			The low power mode is not supported by the device
 * @retval	ERR_FEATURE_NOT_LICENSED	The low power mode is not enabled by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_DisableLowPowerPickUpMode();


/**
 * @brief Disconnects from eyetracking-server. @ref iV_Disconnect will not
 * return until the connection has been disconnected. After this function has
 * been called no other function or device can communicate with eyetracking-server.
 * See also @listsystemstartstop
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_DELETE_SOCKET						failed to delete sockets
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_Disconnect();


/**
 * @brief Enables the low power mode, allowing the eye tracker to lower power consumption when
 * no user is tracked. This can result in longer pick-up time.
 * See also @ref iV_DisableLowPowerPickUpMode
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_IVIEWX_NOT_FOUND		The server is not responsive
 * @retval	ERR_WRONG_DEVICE			The low power is not supported by the device
 * @retval	ERR_FEATURE_NOT_LICENSED	The low power mode is not enabled by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_EnableLowPowerPickUpMode();


/**
 * @brief Enables a CPU high performance mode to prevent the CPU from reducing the performance.
 * See also @ref iV_DisableProcessorHighPerformanceMode
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_EnableProcessorHighPerformanceMode();


/**
 * @brief Updates @ref AccuracyStruct accuracyData with validated accuracy results. Before
 * accuracy data is accessible the accuracy needs to be validated with
 * @ref iV_Validate.
 * See also @listvalidation and the chapter @ref validation in the myGaze SDK Manual.
 *
 * @param	accuracyData							see reference information for @ref AccuracyStruct
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no data available
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_CALIBRATION_NOT_AVAILABLE			system is not calibrated
 * @retval	ERR_CALIBRATION_NOT_VALIDATED			system is not validated
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetAccuracy(AccuracyStruct *accuracyData);


/**
 * @brief Updates <TT>imageData</TT> struct with drawn accuracy results. Before
 * accuracy data is accessible the accuracy needs to be validated with
 * @ref iV_Validate.
 * See also @listvalidation and the chapter @ref validation in the myGaze SDK Manual.
 *
 * @param	imageData								see reference information for @ref ImageStruct
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no new data available
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_CALIBRATION_NOT_AVAILABLE			system is not calibrated
 * @retval	ERR_CALIBRATION_NOT_VALIDATED			system is not validated
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetAccuracyImage(ImageStruct *imageData);


/**
 * @brief Updates stored <TT>calibrationData</TT> information with currently selected parameters.
 * See also @listcalibration
 *
 * @param	calibrationData							see reference information for @ref CalibrationStruct
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCalibrationParameter(CalibrationStruct *calibrationData);


/**
 * @brief Delivers information about a calibration point.
 * See also @listcalibration
 *
 * @param	calibrationPointNumber					number of requested calibration point
 * @param	calibrationPoint						information of requested calibration point, stored in @ref CalibrationPointStruct
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no new data available
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCalibrationPoint(int calibrationPointNumber, CalibrationPointStruct *calibrationPoint);


/**
 * @brief Delivers fixation quality information about a calibration point. If
 * the output data holder is NULL, no data will be returned.
 *
 * @param	calibrationPointNumber					number of requested calibration point
 * @param	left									requested quality information for the left eye, stored in @ref CalibrationPointQualityStruct
 * @param	right									requested quality information for the right eye, stored in @ref CalibrationPointQualityStruct
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCalibrationQuality(int calibrationPointNumber, struct CalibrationPointQualityStruct *left, struct CalibrationPointQualityStruct *right);


/**
 * @brief Updates <TT>imageData</TT> struct with a calibration quality image.
 * A calibration is required before calling this function.
 *
 * @param	imageData								see reference information for @ref ImageStruct
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_CALIBRATION_NOT_AVAILABLE			system is not calibrated
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCalibrationQualityImage(struct ImageStruct *imageData);


/**
 * @brief Updates <TT>calibrationStatus</TT> information.
 * See also @listcalibration
 *
 * @param	calibrationStatus						see reference information for @ref CalibrationStatusEnum
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no new data available
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCalibrationStatus(CalibrationStatusEnum *calibrationStatus);


/**
 * @brief Updates data in @ref CalibrationPointStruct <TT>currentCalibrationPoint</TT>
 * with current calibration point data. See also @listcalibration
 *
 * @param	currentCalibrationPoint					information of requested calibration point, stored in @ref CalibrationPointStruct
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no new data available
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCurrentCalibrationPoint(CalibrationPointStruct *currentCalibrationPoint);


/**
 * @brief Gets the currently loaded myGaze geometry. See chapter @ref geometry in the myGaze SDK Manual
 * and @listgeometry
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCurrentREDGeometry (struct GeometryStruct *geometry);


/**
 * @brief Provides the current eye tracker timestamp in microseconds.
 * See also @listdataacquisition
 *
 * @param	currentTimestamp						requested time stamp
 *
 * @retval RET_SUCCESS								intended functionality has been fulfilled
 * @retval RET_DATA_INVALID							no new data available
 * @retval ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetCurrentTimestamp(__int64 *currentTimestamp);


/**
 * @brief Updates data from @ref EventStruct <TT>eventDataSample</TT> with current event data.
 * See also @listdataacquisition
 *
 * @param	eventDataSample							see reference information for @ref EventStruct
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no new data available
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetEvent(EventStruct *eventDataSample);


/**
 * @brief Updates data from @ref EventStruct32 <TT>eventDataSample</TT> with current event data.
 * See also @listdataacquisition
 *
 * @param	eventDataSample							see reference information for @ref EventStruct32
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no new data available
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetEvent32(EventStruct32 *eventDataSample);


/**
 * @brief Gets the device specific feature key.
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetFeatureKey(__int64 *featureKey);


/**
 * @brief Retrieve gaze quality data.
 * Fills @ref GazeChannelQualityStruct qualityData with validated accuracy results. Before
 * quality data is accessible the system needs to be validated with @ref iV_Validate.
 *
 * @param	qualityData								see reference information for @ref GazeChannelQualityStruct
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no data available
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_CALIBRATION_NOT_AVAILABLE			system is not calibrated
 * @retval	ERR_CALIBRATION_NOT_VALIDATED			system is not validated
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetGazeChannelQuality(struct GazeChannelQualityStruct * qualityData);


/**
 * @brief Gets all available profiles by name. They will be written comma-separated
 * in the char buffer. The user needs to be sure that the buffer is not smaller
 * than the needed buffer length. See chapter @ref geometry
 * in the myGaze SDK Manual and @listgeometry
 *
 * @param	maxSize									the length of the string profileNames
 * @param	profileNames							an empty string where profile names will be put in
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_PARAMETER_INVALIDARAMETER			parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetGeometryProfiles(int maxSize, char *profileNames);


/**
 * @brief Gets the system license expiration date. The license will not expire if the
 * license is set to 00.00.0000.
 * See also @listsystemstartstop
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no new data available
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetLicenseDueDate(DateStruct *licenseDueDate);


/**
 * @brief Gets the geometry data of a requested profile without selecting them.
 * See chapter @ref geometry
 * in the myGaze SDK Manual and @listgeometry
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetREDGeometry (char *profileName, struct GeometryStruct *geometry);


/**
 * @brief Updates data in SampleStruct <TT>rawDataSample</TT> with current eye tracking data.
 * See also @listdataacquisition
 *
 * @param	rawDataSample							see reference information for @ref SampleStruct
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no new data available
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetSample(SampleStruct *rawDataSample);


/**
 * @brief Updates data in SampleStruct32 <TT>rawDataSample</TT> with current eye tracking data sample.
 * See also @listdataacquisition
 *
 * @param	rawDataSample							see reference information for @ref SampleStruct32
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no new data available
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetSample32(SampleStruct32 *rawDataSample);


/**
 * @brief Updated the serial number information of the connected device.
 * See also @listsystemstartstop
 *
 * @param	serialNumber							the serial number of the requested device
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no data available
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
  **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetSerialNumber(char serialNumber[64]);


/**
 * @brief Updates @ref SystemInfoStruct systemInfoData with current system information.
 * See also @listsystemstartstop
 *
 * @param	systemInfoData							see reference information for @ref SystemInfoStruct
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no data available
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetSystemInfo(SystemInfoStruct *systemInfoData);


/**
 * @brief Updates @ref ImageStruct <TT>imageData</TT> with current tracking monitor image
 *
 * @param	imageData								see reference information for @ref ImageStruct
 *
 * @retval RET_SUCCESS								intended functionality has been fulfilled
 * @retval RET_DATA_INVALID							no new data available
 * @retval ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetTrackingMonitor(ImageStruct *imageData);


/**
 * @brief Updates TrackingStatusStruct <TT>trackingStatus</TT> with current tracking status.
 *
 * @param	trackingStatus							see reference information for @ref TrackingStatusStruct
 *
 * @retval RET_SUCCESS								intended functionality has been fulfilled
 * @retval RET_DATA_INVALID							no new data available
 * @retval ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_GetTrackingStatus(TrackingStatusStruct *trackingStatus);


/**
 * @brief Hides accuracy monitor window which can be opened by @ref iV_ShowAccuracyMonitor.
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_WINDOW_IS_CLOSED					window is already closed
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_HideAccuracyMonitor();


/**
 * @brief Hides tracking monitor window which can be opened by @ref iV_ShowTrackingMonitor.
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_WINDOW_IS_CLOSED					window is already closed
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_HideTrackingMonitor();


/**
 * @brief Checks if connection to eyetracking-server is still established.
 * See also @listsystemstartstop
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	RET_WINDOW_IS_CLOSED					window is already closed
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_IsConnected();


/**
 * @brief Checks if the participant will be tracked properly.
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_TRACKING_NOT_STABLE					tracking is not stable
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_IsTrackingStable();


/**
 * @brief Loads a previously saved calibration. A calibration has to be saved by using
 * @ref iV_SaveCalibration.
 * See also @listcalibration
 *
 * @param	name									calibration name / identifier
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_PARAMETER_INVALID					parameter out of range 
 * @retval	ERR_SERVER_NO_RESPONSE					no response from eyetracking-server
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_LoadCalibration(char *name);


/**
 * @brief Suspend the eye tracking application and disables calculation of gaze data.
 * The application can be reactivated by calling @ref iV_ContinueEyetracking.
 * See also @listsystemstartstop
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_PauseEyetracking();


/**
 * @brief Disconnects and closes myGaze eyetracking-server. After this function
 * has been called no other function or application can communicate
 * with eyetracking-server.
 * See also @listsystemstartstop
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_SOCKET_DELETE						failed to delete sockets 
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_Quit();


/**
 * @brief Restarts a calibration procedure with a point from the latest calibration process. The
 * point is specified by its index in the calibration point profile (counted from 1).
 * If the requested point is not found, an error code will be returned.
 * The number of calibration points can be retrieved via @ref iV_GetCalibrationQuality.<BR>
 * This function can be used to improve the final calibration quality in cases when some points
 * from the previous calibration were missed unexpectedly. With this function you can
 * re-include that point to your calibration.<BR>
 * This function follows the workflow of @ref iV_Calibrate except that @ref iV_SetupCalibration
 * must not be called after the end of the last calibration and before calling this function.<BR>
 * If CalibrationStruct::visualization was set to 0, @ref iV_RecalibrateOnePoint
 * returns immediately. The user application has to implement the visualization of calibration points.
 * Information about the current calibration point can be retrieved with
 * @ref iV_GetCurrentCalibrationPoint or with setting up the calibration callback using
 * @ref iV_SetCalibrationCallback.<BR>
 * See also @listcalibration

 * @param	number									number of calibration point to recalibrate
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_PARAMETER_INVALID					the point to recalibrate is not found
 * @retval	ERR_CALIBRATION_NOT_AVAILABLE			no previous calibration exists
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_RecalibrateOnePoint(int number);


/**
 * @brief Resets all calibration points to its default position.
 * See also @listcalibration
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ResetCalibrationPoints();


/**
 * @brief Saves a calibration with a custom name. To save a calibration
 * it is required that a successful calibration already has been completed.
 * See also @listcalibration
 *
 * @param	name									calibration name / identifier
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_CALIBRATION_NOT_AVAILABLE			system is not calibrated
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SaveCalibration(char *name);


/**
 * @brief Selects a predefined myGaze geometry profile. See chapter @ref geometry
 * in the myGaze SDK Manual and @listgeometry
 *
 * @param	profileName								name of the selected profile which should be selected
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SelectREDGeometry(char* profileName);


/**
 * @brief Sets a callback function for the calibration and validation process. The callback function
 * will be called after a calibration or validation point was moved and needs to be drawn,
 * or if the calibration or validation was finished successfully or unsuccessfully. It's possible
 * to disable the callback by hand over NULL as a parameter.
 * See also @listcalibration
 *
 * @param	pCalibrationCallbackFunction			pointer to CalibrationCallbackFunction
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetCalibrationCallback(pDLLSetCalibrationPoint pCalibrationCallbackFunction);


/**
 * @brief Defines a customized timeout for how long @ref iV_Connect tries to connect
 * to myGaze eyetracking server.
 * See also @listsystemstartstop
 *
 * @param	time									the time [sec] iV_Connect is waiting for eyetracking-server response
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetConnectionTimeout(int time);


/**
 * @brief Sets a callback function for the event data. The function will be called if a
 * real-time detected fixation has been started or ended. It's possible
 * to disable the callback by hand over NULL as a parameter.
 * Important note: Algorithms with high processor usage and long calculation time
 * shouldn't run within this callback due to a higher probability of data loss.
 * See also @listdataacquisition
 *
 * @param	pEventCallbackFunction					pointer to EventCallbackFunction
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 * @retval  ERR_FEATURE_NOT_LICENSED				feature not covered by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetEventCallback(pDLLSetEvent pEventCallbackFunction);


/**
 * @brief Defines the detection parameter for online fixation detection algorithm.
 * See also @listdataacquisition
 *
 * @param	minDuration								minimum fixation duration [ms]
 * @param	maxDispersion							maximum dispersion [pixel]
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetEventDetectionParameter(int minDuration, int maxDispersion);


/**
 * @brief Validates the customer license. If provided license is wrong "iV_Connect" refuses to connect.
 * See also @listsystemstartstop
 *
 * @param	licenseKey								provided license key
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetLicense(const char* licenseKey);


/**
* @brief Define the myGaze geometry.
* See chapter @ref geometry in the myGaze SDK Manual
* and @listgeometry for details.
*
*
* @retval	RET_SUCCESS								intended functionality has been fulfilled
* @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
* @retval	ERR_PARAMETER_INVALID					parameter out of range

**/
DLL_DEC_PRE int DLL_DEC_POST iV_SetREDGeometry(struct GeometryStruct *geometry);


/**
 * @brief Sets a callback function for the raw sample data. The function will be called
 * if myGaze eyetracking-server has calculated a new data sample. It's possible
 * to disable the callback by hand over NULL as a parameter.
 * Important note: Algorithms with high processor usage and long calculation time
 * shouldn't run within this callback due to a higher probability of data loss.
 * See also @listdataacquisition
 *
 * @param	pSampleCallbackFunction					pointer to SampleCallbackFunction
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetSampleCallback(pDLLSetSample pSampleCallbackFunction);


/**
 * @brief Sets a callback function for the tracking monitor image data. The function will be
 * called if a new tracking monitor image was calculated. The image format is RGB 24bpp.
 * It's possible to disable the callback by hand over NULL as a parameter.
 * Important note: Algorithms with high processor usage and long calculation time
 * shouldn't run within this callback due to a higher probability of data loss.
 *
 * @param	pTrackingMonitorCallbackFunction		pointer to TrackingMonitorCallbackFunction
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetTrackingMonitorCallback(pDLLSetTrackingMonitor pTrackingMonitorCallbackFunction);


/**
 * @brief Sets myGaze tracking parameters.
 *
 * @param	ET_PARAM_EYE							select specific eye
 * @param	ET_PARAM								select the specific tracking mode
 * @param	reserved								must be set to zero
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 * @retval  ERR_FEATURE_NOT_LICENSED				the features requested by means of
 *													the ET_PARAM parameter is not licensed
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetTrackingParameter(int ET_PARAM_EYE, int ET_PARAM, int reserved);


/**
 * @brief Sets the calibration and validation visualization parameter.
 * See also @listcalibration
 *
 * @param	calibrationData							see reference information for "CalibrationStruct"
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	ERR_PARAMETER_INVALID					parameter out of range
 * @retval	ERR_PARAMETER_CALIBRATION_INVALID		calibration method not supported 
 * @retval  ERR_FEATURE_NOT_LICENSED				at least one of the features requested by means of
 *													the calibrationData parameters are not licensed
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_SetupCalibration(CalibrationStruct *calibrationData);


/**
 * @brief The validated accuracy results will be visualized in a dialog window. Before the
 * image can be drawn the calibration needs to be performed with @ref iV_Calibrate and
 * validated with @ref iV_Validate.
 * See also @listvalidation
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_DATA_INVALID						no data available
 * @retval	RET_WINDOW_IS_OPEN						window is already open
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_CALIBRATION_NOT_AVAILABLE			system is not calibrated
 * @retval	ERR_CALIBRATION_NOT_VALIDATED			system is not validated
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ShowAccuracyMonitor();


/**
 * @brief Visualizes myGaze tracking monitor in a separate dialog window. It shows the position
 * of the participant related to the eye tracking device and indicates (using arrows)
 * if the participant is not positioned in the center of the tracking head box.
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_WINDOW_IS_OPEN						window is already open
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_ShowTrackingMonitor();


/**
 * @brief Starts the myGaze eyetracking-server application. Depending on the PC, it may take
 * several seconds to start the eyetracking-server application. The connection needs
 * to be established separately using @ref iV_Connect. The connection timeout can be
 * extended using @ref iV_SetConnectionTimeout.
 * See also @listsystemstartstop
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_SERVER_IS_RUNNING					eyetracking-server is already running
 * @retval	ERR_SERVER_NOT_FOUND					failed to start myGaze eyetracking-server
 * @retval	ERR_DEVICE_NOT_FOUND					could not detected device
 * @retval	ERR_DEVICE_CONNECTED_TO_WRONG_PORT		USB port not sufficient to access the device
 * @retval	ERR_LICENSE_EXPIRED						the eye tracker license has expired
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_Start();


/**
 * @brief This function causes the eye tracker to temporarily leave the low power
 * to allow a faster pick-up. If the pick-up is not successful, the eye tracker will
 * turn back to its low power state.
 * This function is typically called by an application when it detects some user input
 * and the low power mode should be interrupted immediately.
 *
 * @retval	RET_SUCCESS					intended functionality has been fulfilled
 * @retval	ERR_NOT_CONNECTED			no connection established
 * @retval	ERR_IVIEWX_NOT_FOUND		The server is not responsive
 * @retval	ERR_WRONG_DEVICE			The low power mode is not supported by the device
 * @retval	ERR_FEATURE_NOT_LICENSED	The low power mode is not enabled by your license
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_TriggerLowPowerModeWakeUp();


/**
 * @brief Starts a validation process. To proceed, the participant needs to be tracked and
 * has to fixate on the validation point. Depending on the validation settings (which can
 * be changed using @ref iV_SetupCalibration) the user can accept the validation points
 * manually (by pressing SPACE or calling @ref iV_AcceptCalibrationPoint) or abort the
 * validation (by pressing ESC or calling @ref iV_AbortCalibration). If the validation
 * will be visualized by the API (CalibrationStruct::visualization is set to 1)
 * the function won't return until the validation has been finished (closed automatically)
 * or aborted (ESC).
 *
 * @retval	RET_SUCCESS								intended functionality has been fulfilled
 * @retval	RET_CALIBRATION_ABORTED					validation was aborted during progress
 * @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
 * @retval	ERR_CALIBRATION_NOT_AVAILABLE			system is not calibrated
 **/
DLL_DEC_PRE int DLL_DEC_POST iV_Validate();


/**
* @brief 	Deletes the myGaze geometry setup with the given name. It is not possible
*			to delete a geometry profile if it is currently in use. See chapter
*			@ref geometry in the myGaze SDK Manual.
*
*			Warning! This function is deprecated and will be removed in future releases.
*			Use the function @ref iV_DeleteREDGeometry instead.
*
* @param	setupName								name of the geometry setup which will be deleted
*
* @retval	RET_SUCCESS								intended functionality has been fulfilled
* @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
* @retval	ERR_PARAMETER_INVALID					parameter out of range
**/
DEPRECATED_DLL_DEC_PRE int DLL_DEC_POST iV_DeleteMonitorAttachedGeometry(char *setupName);


/**
* @brief	Gets the currently loaded myGaze geometry. See chapter @ref geometry in the myGaze SDK Manual
*			and @listgeometry
*
*			Warning! This function is deprecated and will be removed in future releases.
*			Use the function @ref iV_GetCurrentREDGeometry instead.
*
* @param	geometry								see reference information for @ref MonitorAttachedGeometryStruct
*
* @retval	RET_SUCCESS								intended functionality has been fulfilled
* @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
**/
DEPRECATED_DLL_DEC_PRE int DLL_DEC_POST iV_GetCurrentMonitorAttachedGeometry(struct MonitorAttachedGeometryStruct *geometry);

/**
* @brief	Gets the geometry data of a requested profile without selecting them.
*			See chapter @ref geometry in the myGaze SDK Manual and @listgeometry
*
*			Warning! This function is deprecated and will be removed in future releases.
*			Use the function @ref iV_GetCurrentREDGeometry instead.
*
* @param	setupName								name of the requested geometry setup
* @param	geometry								see reference information for @ref MonitorAttachedGeometryStruct
*
* @retval	RET_SUCCESS								intended functionality has been fulfilled
* @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
* @retval	ERR_PARAMETER_INVALID					parameter out of range
**/
DEPRECATED_DLL_DEC_PRE int DLL_DEC_POST iV_GetMonitorAttachedGeometry(char *setupName, struct MonitorAttachedGeometryStruct *geometry);

/**
* @brief 	Selects a predefined myGaze geometry profile. See chapter @ref geometry
*			in the myGaze SDK Manual and @listgeometry
*
*			Warning! This function is deprecated and will be removed in future releases.
*			Use the function @ref iV_SelectREDGeometry instead.
*
* @param	profileName									name of the selected profile which should be selected
*
* @return	RET_SUCCESS									intended functionality has been fulfilled
* @return	ERR_CONNECTION_NOT_ESTABLISHED				no connection established
* @return	ERR_PARAMETER_INVALID						parameter out of range
* @return	ERR_SERVER_NO_RESPONSE						no response from eyetracking-server
**/
DEPRECATED_DLL_DEC_PRE int DLL_DEC_POST iV_SetGeometryProfile(char* profileName);


/**
* @brief 	Defines the myGaze monitor attached geometry. See chapter @ref geometry
*			in the myGaze SDK Manual and @listgeometry for details.
*
*			Warning! This function is deprecated and will be removed in future releases.
*			Use the function @ref iV_SetREDGeometry instead.
*
* @param	geometry								see reference information for @ref MonitorAttachedGeometryStruct
*
* @retval	RET_SUCCESS								intended functionality has been fulfilled
* @retval	ERR_CONNECTION_NOT_ESTABLISHED			no connection established
* @retval	ERR_PARAMETER_INVALID					parameter out of range
**/
DEPRECATED_DLL_DEC_PRE int DLL_DEC_POST iV_SetupMonitorAttachedGeometry(struct MonitorAttachedGeometryStruct *geometry);

/** @}*/

