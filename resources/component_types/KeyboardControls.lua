KeyboardControls = {
    speed = 5,
    jump_power = 350,
    controller_index = 0, -- Use the first controller by default
    controller_deadzone = 0.1, -- Ignore small stick movements
    
    OnStart = function(self)
        self.rb = self.actor:GetComponent("Rigidbody")
    end,
    
    OnUpdate = function(self)
        local horizontal_input = 0
        if Input.GetKey("right") then
            horizontal_input = self.speed
        end
        
        if Input.GetKey("left") then
            horizontal_input = -self.speed
        end
        
        if Input.IsControllerConnected(self.controller_index) then
            local stick_x = Input.GetAxis(self.controller_index, "leftx")
            
            if math.abs(stick_x) > self.controller_deadzone then
                horizontal_input = stick_x * self.speed
            end
            
            if Input.GetButton(self.controller_index, "dpad_right") then
                horizontal_input = self.speed
            end
            
            if Input.GetButton(self.controller_index, "dpad_left") then
                horizontal_input = -self.speed
            end
        end
        
        local vertical_input = 0
        
        local on_ground = false
        ground_object = Physics.Raycast(self.rb:GetPosition(), Vector2(0, 1), 1)
        
        if Input.GetKeyDown("up") or Input.GetKeyDown("space") then
            if ground_object ~= nil then
                vertical_input = -self.jump_power
            end
        end
        
        if Input.IsControllerConnected(self.controller_index) then
            if Input.GetButtonDown(self.controller_index, "a") or 
               Input.GetButtonDown(self.controller_index, "b") then
                if ground_object ~= nil then
                    vertical_input = -self.jump_power
                    
                    Input.SetVibration(self.controller_index, 0.3, 0.3, 100)
                end
            end
        end
        
        self.rb:AddForce(Vector2(horizontal_input, vertical_input))
    end
}