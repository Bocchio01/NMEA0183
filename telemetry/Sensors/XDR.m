classdef XDR < handle
    % XDR Sensor class

    properties
        data
        fig
        plotter
        annotation
    end

    methods
        function XDR = XDR(fig)
            
            XDR.data = struct( ...
                'type', 0.0, ...
                'temperature', 0.0, ...
                'unit', '', ...
                'name', '');

            if ~ishandle(fig)
                fig = figure('Name', 'XDR Telemetry', 'NumberTitle', 'off');
            end
            XDR.fig = fig;

            nexttile
            XDR.plotter = plot(0, 0, "-", ...
                "LineWidth", 2);
            grid on
            title('XDR')
            legend('Temperature')

            % XDR.annotation = annotation('textbox', ...
            %     [0.5 .7 .3 .3], ...
            %     'Units', 'normalized', ...
            %     'FitBoxToText', 'on');

        end

        function [XDR] = Parser(XDR, data_from_STM)

            typePattern = 'Type:\s+(\w)';
            temperaturePattern = 'Temp.:\s+([-+]?\d*\.\d+)';
            unitPattern = 'Unit:\s+(\w+)';
            namePattern = 'Name:\s+(\w*)';

            typeMatch = regexp(data_from_STM, typePattern, 'tokens', 'once');
            temperatureMatch = regexp(data_from_STM, temperaturePattern, 'tokens', 'once');
            unitMatch = regexp(data_from_STM, unitPattern, 'tokens', 'once');
            nameMatch = regexp(data_from_STM, namePattern, 'tokens', 'once');

            try
                XDR.data.type = typeMatch{1};
                XDR.data.temperature = str2double(temperatureMatch{1});
                XDR.data.unit = unitMatch{1};
                XDR.data.name = nameMatch{1};
            catch
                disp('Error reading for XDR');
            end
        end


        function [] = Plotter(XDR)

            y = XDR.data.temperature;

            XDR.plotter.XData = [XDR.plotter.XData max(XDR.plotter.XData + 1)];
            XDR.plotter.YData = [XDR.plotter.YData y];

            % annotation_text = sprintf('XDR Data:\nType: %s\nTemperature: %.2f %s\nName: %s', ...
            %     XDR.data.type, ...
            %     XDR.data.temperature, ...
            %     XDR.data.unit, ...
            %     XDR.data.name);
            % 
            % set(XDR.annotation, 'String', annotation_text)

            drawnow;

        end
    end
end

