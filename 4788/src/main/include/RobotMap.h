#pragma once

#include <string>

#include "devices/StateDevice.h"
#include "control/PIDController.h"
#include "strategy/StrategySystem.h"
#include "control/MotorFilters.h"
#include "Gearbox.h"
#include "strategy/Strategy.h"
#include "controllers/Controllers.h"
#include "sensors/BinarySensor.h"

// FRC
#include <frc/SpeedControllerGroup.h>
#include <frc/Spark.h>
#include <frc/PowerDistributionPanel.h>
#include <frc/PWMSparkMax.h>

// REV
#include "rev/CANSparkMax.h"


#include "WMLCtre.h"
#include "controllers/Controllers.h"
#include "actuators/BinaryServo.h"
#include "actuators/Compressor.h"
#include "actuators/DoubleSolenoid.h"
#include "actuators/VoltageController.h"
#include "Drivetrain.h"
#include "sensors/Encoder.h"
#include "sensors/LimitSwitch.h"
#include "sensors/NavX.h"
#include "sensors/PressureSensor.h"
#include <networktables/NetworkTableInstance.h>
#include "control/PIDController.h"
#include "MotionProfiling.h"
#include "Toggle.h"

#include "Usage.h"

// Local Files
#include "ControlMap.h"


struct RobotMap {

  // Controllers
  #if __CONTROLMAP_USING_JOYSTICK__
  
  wml::controllers::Joystick joy1{ ControlMap::JoyController1Port };
  wml::controllers::Joystick joy2{ ControlMap::JoyController2Port };
  wml::controllers::SmartControllerGroup contGroup{ joy1, joy2 };
  #else

  wml::controllers::XboxController xbox1{ ControlMap::XboxController1Port };
  wml::controllers::XboxController xbox2{ ControlMap::XboxController2Port };
  wml::controllers::SmartControllerGroup contGroup{ xbox1, xbox2 };
  #endif

  // Drive System
  struct DriveSystem {
    // Front
    wml::TalonSrx FLmax{ ControlMap::DriveMAXportFL };
    wml::TalonSrx FRmax{ ControlMap::DriveMAXportFR };

    // Back
    rev::CANSparkMax BLmax{ 1, rev::CANSparkMax::MotorType::kBrushed };
    rev::CANSparkMax BRmax{ 4, rev::CANSparkMax::MotorType::kBrushed };

    // @TODO: Add encoders to drivetrain gearboxes (Will do when we have neo's... or if we have neo's... they may be on fire by the time they get here. Whatever)

    wml::Gearbox LGearbox{ new wml::actuators::MotorVoltageController(wml::actuators::MotorVoltageController::Group(FLmax, BLmax)), nullptr };
    wml::Gearbox RGearbox{ new wml::actuators::MotorVoltageController(wml::actuators::MotorVoltageController::Group(FRmax, BRmax)), nullptr };

    wml::actuators::DoubleSolenoid ChangeGearing{ ControlMap::ChangeGearPort1, ControlMap::ChangeGearPort2, ControlMap::ChangeGearTime };

    wml::DrivetrainConfig driveTrainConfig{ LGearbox, RGearbox };
    wml::Drivetrain drivetrain{ driveTrainConfig };
    wml::control::PIDGains gainsVelocity{ "Drivetrain Velocity", 1 };
  };
  DriveSystem driveSystem;

  struct Turret {
    wml::sensors::LimitSwitch LeftLimit{ ControlMap::TurretLeftLimitPort, ControlMap::TurretLeftLimitInvert };
    wml::sensors::LimitSwitch RightLimit{ ControlMap::TurretRightLimitPort, ControlMap::TurretRightLimitInvert };
    wml::sensors::LimitSwitch AngleDownLimit{ ControlMap::TurretAngleDownLimitPort, ControlMap::TurretAngleDownLimitInvert };

    wml::VictorSpx TurretRotation{ ControlMap::TurretRotationPort };
    wml::TalonSrx TurretAngle{ ControlMap::TurretRotationPort };

    wml::Gearbox turretRotation{ new wml::actuators::MotorVoltageController(wml::actuators::MotorVoltageController::Group(TurretRotation)), nullptr };
    wml::Gearbox turretAngle{ new wml::actuators::MotorVoltageController(wml::actuators::MotorVoltageController::Group(TurretAngle)), nullptr };

    // Fly Wheel
    wml::TalonSrx TurretFlyWheel{ ControlMap::TurretFlyWheelPort, 2048 };
    wml::actuators::MotorVoltageController flywheelMotors = wml::actuators::MotorVoltageController::Group(TurretFlyWheel);
    wml::Gearbox turretFlyWheel{ &flywheelMotors, &TurretFlyWheel, 8.45 };
  };
  Turret turret;

  struct Intake {
    wml::TalonSrx IntakeMotor{ ControlMap::IntakeMotorPort };
    wml::actuators::DoubleSolenoid IntakeDown { ControlMap::IntakeDownPort1, ControlMap::IntakeDownPort2 , ControlMap::PannelActuationTime};
    
    wml::Gearbox intakeMotor{ new wml::actuators::MotorVoltageController(wml::actuators::MotorVoltageController::Group(IntakeMotor)), nullptr };
  };
  Intake intake;

  struct MagLoader {
    wml::TalonSrx MagLoaderMotor{ ControlMap::MagLoaderMotorPort };

    wml::sensors::LimitSwitch StartMagLimit{ ControlMap::StartMagLimitPort };
    wml::sensors::LimitSwitch Position1Limit{ ControlMap::Position1LimitPort };
    wml::sensors::LimitSwitch Position5Limit{ ControlMap::Position5LimitPort };

    wml::Gearbox magLoaderMotor{ new wml::actuators::MotorVoltageController(wml::actuators::MotorVoltageController::Group(MagLoaderMotor)), nullptr };
  };
  MagLoader magLoader;

  struct ControlPannel {
    wml::TalonSrx MotorControlPannel{ ControlMap::ControlPannelPort };
    wml::actuators::DoubleSolenoid PannelPnSol { ControlMap::ControlPannelPort, ControlMap::PannelActuatorPort1, ControlMap::PannelActuationTime};

    wml::Gearbox ControlPannelMotor { new wml::actuators::MotorVoltageController(wml::actuators::MotorVoltageController::Group(MotorControlPannel)), nullptr };
  };
  ControlPannel controlPannel;

  struct Climber {
    wml::actuators::DoubleSolenoid ClimberActuator{ ControlMap::ClimberActuatorPort1, ControlMap::ClimberActuatorPort2, ControlMap::ClimberActuationTime };

    wml::actuators::BinaryServo ShiftPTOServos{ ControlMap::Shift2PTOPort, ControlMap::Shift2PTOForwardPosition, ControlMap::Shift2PTOReversePosition };
    wml::TalonSrx Climber1Motor{ ControlMap::ClimberMotor1Port };
    wml::TalonSrx Climber2Motor{ ControlMap::ClimberMotor2Port };

    wml::Gearbox ClimberElevator{ new wml::actuators::MotorVoltageController(wml::actuators::MotorVoltageController::Group(Climber1Motor, Climber2Motor)), nullptr };
  };
  Climber climber;

  struct ControlSystem {
    wml::sensors::PressureSensor pressureSensor{ ControlMap::PressureSensorPort };
    wml::actuators::Compressor compressor{ ControlMap::CompressorPort }; 

    // Vision Tracking Values Sent from the coprocessor (pi/tinkerboard)
    std::shared_ptr<nt::NetworkTable> visionTable = nt::NetworkTableInstance::GetDefault().GetTable("VisionTracking");
    //std::shared_ptr<nt::NetworkTable> table = visionTable->GetSubTable("Target");
    //double targetX = table->GetNumber("Target_X", 0), targetY = table->GetNumber("Target_Y", 0), imageHeight = table->GetNumber("ImageHeight", 0), imageWidth = table->GetNumber("ImageWidth", 0);

  };
  ControlSystem controlSystem;


};