function start()
  print("hello from", entity.getname(self))
end

function update()
  x,y,z = entity.getrot(self)
  entity.setrot(self,x,y+0.5,z)
end
