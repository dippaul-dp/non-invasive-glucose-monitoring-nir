% =========================================================================
% Project: Non-Invasive Glucose Monitoring System Using Phototransistor Sensor
% Paper  : IEEE SPICSCON 2025 (DOI: 10.1109/SPICSCON69221.2025.11504207)
% Script : Calibration curve fitting and R^2 evaluation
% =========================================================================

clc; clear; close all;

% --- Non-Diabetic Dataset (N=15) ---
non_diab_adc = [509, 491, 535, 483, 494, 405, 434, 456, 444, 425, 443, 440, 420, 438, 400];
non_diab_ref = [6.8, 6.6, 6.9, 6.5, 6.7, 5.0, 6.5, 5.7, 5.9, 6.8, 6.3, 6.3, 5.4, 6.5, 5.1];

% --- Diabetic Dataset (N=10) ---
diab_adc = [389, 380, 368, 381, 374, 397, 390, 370, 367, 371];
diab_ref = [8.8, 9.8, 10.8, 11.4, 8.6, 11.6, 11.7, 8.5, 10.6, 8.4];

% Combined Dataset
all_adc = [non_diab_adc, diab_adc];
all_ref = [non_diab_ref, diab_ref];

% Predicted values from piecewise model
predicted_glucose = zeros(size(all_adc));
for i = 1:length(all_adc)
    if all_adc(i) > 400
        predicted_glucose(i) = 0.0115 * all_adc(i) + 0.9864;
    else
        predicted_glucose(i) = 0.0564 * all_adc(i) - 11.3220;
    end
end

% Calculate Coefficient of Determination (R^2)
R = corrcoef(all_ref, predicted_glucose);
R2 = R(1,2)^2;
fprintf('Overall Coefficient of Determination (R^2): %.4f\n', R2);

% Plot
figure('Color', [1 1 1]);
scatter(all_adc, all_ref, 40, 'r', 'filled'); hold on;
plot(sort(all_adc), sort(predicted_glucose), 'b-', 'LineWidth', 1.5);
xlabel('Mean Sensor ADC Value');
ylabel('Invasive Glucose (mmol/L)');
title(sprintf('Optical Sensor Response vs. Invasive Gold Standard (R^2 = %.4f)', R2));
legend('Experimental Data Points', 'Regression Calibration Fit', 'Location', 'best');
grid on;
