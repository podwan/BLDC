clc;
clear all;
%% Set PWM Switching frequency
PWM_frequency 	= 20e3;    %Hz          // converter s/w freq
T_pwm           = 1/PWM_frequency;  %s  // PWM switching time period

%% Set Sample Times
Ts          	= T_pwm;        %sec        // Sample time step for controller
Ts_simulink     = T_pwm/2;      %sec        // Simulation time step for model simulation
Ts_motor        = T_pwm/2;      %Sec        // Simulation sample time
Ts_inverter     = T_pwm/2;      %sec        // Simulation time step for average value inverter
Ts_speed        = 10*Ts;        %Sec        // Sample time for speed controller

%% Set data type for controller & code-gen
%dataType = fixdt(1,32,17);    % Fixed point code-generation
dataType = 'single';            % Floating point code-generation

% %% System Parameters // Hardware parameters
% 
% % Set Target Parameters
% target.CPU_frequency        = 64e6;					%Hz     // Clock frequency
% target.PWM_frequency        = PWM_frequency;   		%Hz     // PWM frequency
% target.PWM_Counter_Period   = round(target.CPU_frequency/target.PWM_frequency/2); % //PWM timer counts for up-down counter
% target.ADC_Vref             = 3.3;					%V		// ADC voltage reference for LAUNCHXL-F28379D
% target.ADC_MaxCount         = 4095;					%		// Max count for 12 bit ADC
% target.SCI_baud_rate        = 4e6;                 %Hz     // Set baud rate for serial communication

% BR2804-1700

pmsm.model             ='BR2804-1700';
pmsm.sn                ='008';
pmsm.p                 =7;
pmsm.Rs                =0.5300;
pmsm.Ld                =3.7e-05;
pmsm.Lq                =3.7e-05;
pmsm.J                 =1.95e-07;
pmsm.B                 =2.53e-07;
pmsm.N_max             =18000;
pmsm.Ke                =0.600;
pmsm.Kt                =4.96e-03;
pmsm.FluxPM            =4.73e-04;
pmsm.I_rated           =5.6;
pmsm.N_base            =12540;
pmsm.F_base            =pmsm.N_base*pmsm.p/60;
pmsm.DC                =12;
pmsm.T_rated           = (3/2)*pmsm.p*pmsm.FluxPM*pmsm.I_rated;

Vmin                   =1;


Kp_C=pmsm.Lq/(3*Ts);
Ki_C=pmsm.Rs/(3*Ts);

Kp_S=(pi*pmsm.J)/(45*4*Ts*10^(2.5/2)*pmsm.p*pmsm.FluxPM);
Ki_S=(pi*pmsm.J)/(45*4*Ts*10^(2.5/2)*pmsm.p*pmsm.FluxPM*4*Ts*10^(2.5));


% Interval
inverter.model         = 'X-NUCLEO-IHM07M1'; 	% 		// Manufacturer Model Number
inverter.sn            = 'INV_XXXX';         	% 		// Manufacturer Serial Number
inverter.V_dc          = 24;       				%V      // DC Link Voltage of the Inverter
inverter.I_trip        = 10;       				%Amps   // Max current for trip
inverter.Rds_on        = 2e-3;     				%Ohms   // Rds ON for X-NUCLEO-IHM07M1
inverter.Rshunt        = 0.33;    				%Ohms   // Rshunt for X-NUCLEO-IHM07M1
inverter.CtSensAOffset = 1850;        			%Counts // ADC Offset for phase-A
inverter.CtSensBOffset = 1857;        			%Counts // ADC Offset for phase-B
inverter.ADCGain       = 1;                     %       // ADC Gain factor scaled by SPI
inverter.EnableLogic   = 1;    					% 		// Active high for X-NUCLEO-IHM07M1 enable pin (EN_GATE)
inverter.invertingAmp  = -1;   					% 		// Currents entering motor phases are read as negative values in this hardware
inverter.ISenseVref    = 3.3;					%V 		// Voltage ref of inverter current sense circuit
inverter.ISenseVoltPerAmp = 0.505; 				%V/Amps // Current sense voltage output per 1 A current (Rshunt * iSense op-amp gain)
inverter.ISenseMax     = inverter.ISenseVref/(2*inverter.ISenseVoltPerAmp); %Amps // Maximum Peak-Neutral current that can be measured by inverter current sense
inverter.R_board        = inverter.Rds_on + inverter.Rshunt/3;  %Ohms

% %% Derive Characteristics
% pmsm.N_base = mcb_getBaseSpeed(pmsm,inverter); %rpm // Base speed of motor at given Vdc

% %% PU System details // Set base values for pu conversion
% PU_System = mcb_SetPUSystem(pmsm,inverter);

% %% Controller design // Get ballpark values!
% PI_params = mcb.internal.SetControllerParameters(pmsm,inverter,PU_System,T_pwm,5*Ts,Ts_speed);

% %Updating delays for simulation
% PI_params.delay_Currents    = int32(Ts/Ts_simulink);
% PI_params.delay_Position    = int32(Ts/Ts_simulink);
% PI_params.delay_Speed       = int32(Ts_speed/Ts_simulink);
% PI_params.delay_Speed1       = (PI_params.delay_IIR + 0.5*Ts)/Ts_speed;
