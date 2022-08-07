function update()
  if input.getkeydown("e") then
    if not impulse_applied then
      rb.applyimpulse(self,0,50,0)
    end
    impulse_applied = true
  else
    impulse_applied = false
  end
end
