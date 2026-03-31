#!/bin/bash
set -e

# NIX labs ESPHome Webserver Branding Script
# This script clones the esphome-webserver repo, applies NIX labs branding,
# and builds a single-file www.js.

REPO_URL="https://github.com/esphome/esphome-webserver.git"
TEMP_DIR="tmp_esphome_webserver"
BRAND_COLOR="#FF9100"
BG_COLOR="#101828"

echo "--- Starting NIX labs branding process ---"

# 1. Clone repository
if [ -d "$TEMP_DIR" ]; then
    echo "Cleaning up existing temp directory..."
    rm -rf "$TEMP_DIR"
fi

echo "Cloning esphome-webserver..."
git clone --depth 1 "$REPO_URL" "$TEMP_DIR"

cd "$TEMP_DIR/packages/v3"

# 2. Define Branding CSS
echo "Preparing NIX labs branding CSS..."
BRANDING_CSS="
/* NIX labs Branding Overrides */
@import url('https://fonts.googleapis.com/css2?family=Exo+2:wght@700&family=Inter:wght@400;700&display=swap');

:root {
  --primary-color: $BRAND_COLOR !important;
  --secondary-color: $BRAND_COLOR !important;
  --c-primary: $BRAND_COLOR !important;
  --c-primary-light: $BRAND_COLOR !important;
  --c-bg: $BG_COLOR !important;
}

html, body {
  font-family: 'Inter', sans-serif !important;
  background-color: $BG_COLOR !important;
}

/* Force override hardcoded blue in buttons */
button, .btn {
  color: $BRAND_COLOR !important;
  font-weight: 500 !important;
}

button:hover, .btn:hover {
  background-color: rgba(255, 145, 0, 0.1) !important;
}

/* Force override hardcoded blue in entity icons */
.entity-row > :nth-child(1) {
  color: $BRAND_COLOR !important;
}

/* Header styling */
header h1 {
  font-family: 'Exo 2', sans-serif !important;
  font-weight: 700 !important;
  color: $BRAND_COLOR !important;
}

header iconify-icon, header #logo {
  color: $BRAND_COLOR !important;
}

"

# 3. Hard Remove Theme Toggle from Header
echo "Removing theme toggle from header..."
# Find and remove the <a id="scheme">...</a> block in esp-app.ts
perl -i -0777 -pe 's/<a\s+href="#"\s+id="scheme".*?<\/a>//gs' src/esp-app.ts

# 4. Force NIX labs branding in source files
echo "Redefining colors and fonts directly in component source code..."
# Replace hardcoded fonts in css files
find src -type f \( -name "*.ts" -o -name "*.css" \) -exec sed -i '' "s/ui-monospace, system-ui, \"Helvetica\", \"Arial Narrow\", \"Roboto\", \"Oxygen\", \"Ubuntu\", sans-serif/'Inter', sans-serif/g" {} +

# Force Exo 2 and Orange on h1 in src/css/app.ts
sed -i '' "s/h1 {/h1 { font-family: 'Exo 2', sans-serif !important; color: $BRAND_COLOR; /g" src/css/app.ts

# Also target icon colors in header
sed -i '' "s/color: rgba(127, 127, 127, 0.5);/color: $BRAND_COLOR;/g" src/css/app.ts

# Set tab-header font and color (White) in src/css/tab.ts
sed -i '' "s/.tab-header {/.tab-header { font-family: 'Exo 2', sans-serif !important; color: #ffffff !important; /g" src/css/tab.ts

# FIX: Force charts to the front (z-index: 10) with no pointer events so they are visible above backgrounds
sed -i '' "s/z-index: -100;/z-index: 10; pointer-events: none;/g" src/esp-entity-chart.ts

# FIX: Increase chart opacity for better visibility
sed -i '' 's/this\.style\.opacity = "0\.5";/this.style.opacity = "0.8";/g' src/esp-entity-chart.ts
sed -i '' 's/this\.style\.opacity = "0\.1";/this.style.opacity = "0.5";/g' src/esp-entity-chart.ts

# 5. Deep Clean: Replace hardcoded blue colors in all source files
echo "Replacing hardcoded blue colors in source files..."
# Targets: #03a9f4, rgb(3, 169, 244), #44739e, #26a69a
find src -type f \( -name "*.ts" -o -name "*.css" \) -exec sed -i '' "s/#03a9f4/$BRAND_COLOR/g" {} +
find src -type f \( -name "*.ts" -o -name "*.css" \) -exec sed -i '' "s/rgb(3, 169, 244)/rgb(255, 145, 0)/g" {} +
find src -type f \( -name "*.ts" -o -name "*.css" \) -exec sed -i '' "s/#44739e/$BRAND_COLOR/g" {} +
find src -type f \( -name "*.ts" -o -name "*.css" \) -exec sed -i '' "s/#26a69a/$BRAND_COLOR/g" {} +

# 6. Inject Global CSS Styles and persistent Font Link in main.ts
echo "Injecting final Font and global style logic..."
# We define a very thorough branding CSS
BP_BRANDING_CSS="
/* NIX labs Global Style Injection */
:root {
  --primary-color: $BRAND_COLOR !important;
  --c-primary: $BRAND_COLOR !important;
  --c-bg: $BG_COLOR !important;
}
html, body {
  background-color: $BG_COLOR !important;
  color-scheme: dark !important;
  font-family: 'Inter', sans-serif !important;
}
/* Force Exo 2 and Orange on Headings */
h1, header h1, .dashboard-title {
  font-family: 'Exo 2', sans-serif !important;
  font-weight: 700 !important;
  color: $BRAND_COLOR !important;
}
/* Force Exo 2 on Tab Headers but keep White */
.tab-header {
  font-family: 'Exo 2', sans-serif !important;
  font-weight: 700 !important;
  color: #ffffff !important;
}
"

# Prepend the injector to main.ts to ensure it runs before Lit rendering
mv src/main.ts src/main.ts.bak
cat <<EOF > src/main.ts
// NIX labs branding injection
(function() {
  const fontLink = document.createElement('link');
  fontLink.rel = 'stylesheet';
  fontLink.href = 'https://fonts.googleapis.com/css2?family=Exo+2:wght@700&family=Inter:wght@400;700&display=swap';
  document.head.appendChild(fontLink);

  const style = document.createElement('style');
  style.textContent = \`$BP_BRANDING_CSS\`;
  document.head.appendChild(style);
})();

EOF
cat src/main.ts.bak >> src/main.ts
rm src/main.ts.bak

# 7. Install dependencies and build
echo "Installing dependencies..."
npm install

echo "Building branded webserver..."
npm run build

# 6. Extract results
echo "Locating build output..."
BUILD_OUT="../../_static/v3"

if [ -f "$BUILD_OUT/www.js" ]; then
    echo "Found www.js. Updating static/www.js..."
    cp "$BUILD_OUT/www.js" "../../../static/www.js"
    echo "Successfully updated static/www.js"
else
    # Fallback: check dist/assets or similar
    JS_FILE=$(find dist -name "index-*.js" | head -n 1)
    if [ -n "$JS_FILE" ]; then
         cp "$JS_FILE" "../../../static/www.js"
         echo "Successfully updated static/www.js from $JS_FILE"
    else
        echo "Error: Could not find build output www.js!"
        ls -R ../..
        exit 1
    fi
fi

echo "--- Branding complete! ---"
echo "You can now find the branded interface in static/www.js"
echo "To repeat this in the future, just run: bash scripts/brand_webserver.sh"
