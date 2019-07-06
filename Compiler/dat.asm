.data 
; SUPPORTING VARIABLES 
@buffer   db		6 
blength   db      (?) 
@buf      db      256 DUP (?) 
clrf      db      0Dh, 0Ah, "$" 
output    db      6 DUP (?), "$" 
err_msg   db      "Input error, try again", 0Dh, 0Ah, "$" 
@true     db      "true" 
@@true    db      "true$" 
@false    db      "false" 
@@false   db      "false$" 
; USING VARIABLES 
p1 dw  (?) 
p2 dw  (?) 
x dw  (?) 
i dw  (?) 
j dw  (?) 
k dw  (?) 
z dw  5 DUP (?) 
y dw  5 DUP (?) 
h db  (?) 
e db  (?) 
l db  (?) 
f db  (?) 
@30p db "'do write'$" 
@37p db "'enter bool'$" 
@48p db "'text'$" 
@49p db "'another text'$" 
@50p db "'enter char and int'$" 
@74p db "'if then'$" 
@75p db "'if else'$" 
@80p db "'x != 5'$" 
@87p db "'x > 0 or x == 5'$" 
