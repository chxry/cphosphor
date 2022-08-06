r,g,b = light.getcolor(self)
if r > 0 and eq(b,0) then
  r=r-0.01
  g=g+0.01
end
if g > 0 and eq(r,0) then
  g=g-0.01
  b=b+0.01
end
if b > 0 and eq(g,0) then
  r=r+0.01
  b=b-0.01
end
light.setcolor(self,r,g,b)