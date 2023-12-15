%% comunicazione seriale 

% Creazione di un oggetto serialport con il baud rate specificato
seriale = serialport('COM7', 4800); % Sostituisci 'COM3' con la tua porta seriale corretta
signal=['$','W','I','X','D','R',',','C',',','0','2','7','.','0',',','C',',',',','*','5','7'];
% Dati da inviare all'STM32
for i = 1:length(signal)
    dati_da_inviare = signal(i);
    % Invio dei dati all'STM32
    write(seriale, dati_da_inviare, 'char'); % 'char' specifica che i dati sono caratteri
    pause(0.01)
end 
% Chiudi la porta seriale quando hai finito
clear seriale; % Chiude e elimina l'oggetto serialport