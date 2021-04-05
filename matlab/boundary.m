function S = boundary(p, shapeIn, cnts) 
% S = BOUNDARY(p,shapeIn) - returns the point samples form the desired shape.
% S = BOUNDARY(opts) - returns the point samples form the parameters defined in opts.
%
% Given the number of discretization points (p), and the desired shape, it
% returns the sample points on a Gauss-uniform grid. For instance a grid
% generated by PARDOMAIN. The output format is of class vesicle.
%
% SEE ALSO: PARDOMAIN, VESICLE.
%

  shape = '';
  if(nargin>1), shape = shapeIn;end
  kappa = 0.01;
  if(isstruct(p))
    shp = p.shape;
    viscCont = p.viscCont;
    p = p.p;
  else
    shp = shape;
    viscCont = 1;
  end

  [u v] = parDomain(p);
  spherical = false;
  ax = 1; ay = 1; az = 1;

  switch shp
   case 'ellipseZ'
    rho = 1;
    ax = 1/2; ay = 1/2;
    spherical = true;

   case 'ellipseX'
    rho = 1;
    ay = .34;
    az = .34;
    spherical = true;

   case 'dumbbell'
    rho = 1 + real(Ynm(2,0,u,v));
    spherical = true;
   case 'tiltDumbbell'	
    rho = 1 + real(Ynm(2,1,u,v)) + .1*real(Ynm(3,2,u,v));
    spherical = true;
   case 'neck'
    %rho = .1+abs(Ynm(1,0,u,v)).^2;
    if(isstruct(shape) && isfield(shape,'cf'))
      cf = shape.cf;
    else
      cf = .8;
    end
    z = cos(u);
    s = 1-cf*cos(pi*(z-.2)/2); 
    %constriction factor, tweak the coefficient of
    %the cos to get a slimmer/wider neck.
    r = sqrt((1-z.^2).*s.^2);
    [x y] = pol2cart(v,r);
    S = vesicle([x;y;z], kappa, 1, 1, viscCont);
   case 'fourBump'
    rho = 1 + .2*exp(-3*real(Ynm(3,2,u,v)));
    spherical = true;
   case 'eightBump'
    rho = exp(.5*sin(u).^4.*cos(u).*cos(4*v));
    spherical = true;
   case 'oblate85'
    rho = 1;
    ax = .47;
    spherical = true;
   case 'oblate75'
    rho = 1;
    ax = .366;
    spherical = true;

   case 'oblate65'
    rho = 1;
    ax = .29;
    spherical = true;

   case 'parachute'
    load('parachute65.mat');
    S.resample(p);
    S.kappa = kappa;
    S.viscCont = viscCont;
    
   otherwise
    rho = 1;
    a = 1;
    spherical = true;
  end

  if(spherical)
    X = ax*rho.*sin(u).*cos(v);
    Y = ay*rho.*sin(u).*sin(v);
    Z = az*rho.*cos(u);
    S = vesicle([X;Y;Z], kappa, 1, 1, viscCont(1));
 end 

 if(nargin>2 && ~isempty(cnts))
   cnts = reshape(cnts,[],3);
   X = d3Vec();
   if(length(viscCont)<size(cnts,1))
     viscCont = repmat(viscCont(1), size(cnts,1), 1);
   end
   
   for ii=[2:size(cnts,1) 1]
     X.x = S(1).cart.x + cnts(ii, 1);
     X.y = S(1).cart.y + cnts(ii, 2);
     X.z = S(1).cart.z + cnts(ii, 3);
     S(ii) = vesicle( X, kappa, 1, 1, viscCont(ii));
   end
 end