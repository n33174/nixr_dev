#!/bin/bash
set -e

# NIX labs ESPHome Webserver Branding Script
# This script clones the esphome-webserver repo, applies NIX labs branding,
# and builds a single-file www.js for use in the AL60 ESPHome firmware.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_URL="https://github.com/esphome/esphome-webserver.git"
TEMP_DIR="$SCRIPT_DIR/../.build_tmp/esphome_webserver"
BRAND_COLOR="#FF9100"
BG_BASE="#101828"
BG_CARD="rgba(106, 114, 130, 0.1)"
STATIC_OUT="$SCRIPT_DIR/../static"

# Clean up temp dir on script exit (success or failure)
trap 'echo "Cleaning up temp directory..."; rm -rf "$TEMP_DIR"' EXIT

echo "--- Starting NIX labs branding process ---"

# 1. Clone repository
if [ -d "$TEMP_DIR" ]; then
    echo "Cleaning up existing temp directory..."
    rm -rf "$TEMP_DIR"
fi

echo "Cloning esphome-webserver..."
git clone --depth 1 "$REPO_URL" "$TEMP_DIR"

cd "$TEMP_DIR/packages/v3"

# 2. Remove theme toggle from header
echo "Removing theme toggle from header..."
perl -i -0777 -pe 's/<a\s+href="#"\s+id="scheme".*?<\/a>//gs' src/esp-app.ts
if grep -q 'id="scheme"' src/esp-app.ts; then
    echo "WARNING: theme toggle may not have been removed — upstream source may have changed"
fi

# 3. Force NIX labs branding in source files
echo "Redefining colors and fonts in component source..."

# Replace hardcoded fonts
find src -type f \( -name "*.ts" -o -name "*.css" \) -exec sed -i '' "s/ui-monospace, system-ui, \"Helvetica\", \"Arial Narrow\", \"Roboto\", \"Oxygen\", \"Ubuntu\", sans-serif/'Inter', sans-serif/g" {} +

# Exo 2 and brand orange on h1
sed -i '' "s/h1 {/h1 { font-family: 'Exo 2', sans-serif !important; color: $BRAND_COLOR; /g" src/css/app.ts

# Brand orange on header icon color
sed -i '' "s/color: rgba(127, 127, 127, 0.5);/color: $BRAND_COLOR;/g" src/css/app.ts

# Tab container: glass card bg + blur (Shadow DOM — must modify source directly, injected CSS cannot pierce it)
# Note: .tab-container and .tab-header are inside Lit component shadow roots
# border-radius is intentionally not overridden — the default keeps the top-left corner square
sed -i '' "s/\.tab-container {/\.tab-container { background: $BG_CARD !important; box-shadow: 0 4px 24px rgba(0, 0, 0, 0.5) !important; backdrop-filter: blur(8px) !important; /g" src/css/tab.ts

# Tab header: Exo 2, white, with blur (also inside Shadow DOM)
sed -i '' "s/\.tab-header {/\.tab-header { font-family: 'Exo 2', sans-serif !important; color: #ffffff !important; backdrop-filter: blur(8px) !important; /g" src/css/tab.ts

# Entity rows: padding and spacing between rows (inside Shadow DOM of esp-entity-table — must be done at source level)
sed -i '' "s/\.entity-row {/\.entity-row { padding: 0.5rem 0.75rem !important; margin: 4px 0 !important; /g" src/css/esp-entity-table.ts

# FIX: Force charts above glass card backgrounds so they remain visible
sed -i '' "s/z-index: -100;/z-index: 10; pointer-events: none;/g" src/esp-entity-chart.ts

# FIX: Increase chart opacity for better readability on dark backgrounds
sed -i '' 's/this\.style\.opacity = "0\.5";/this.style.opacity = "0.8";/g' src/esp-entity-chart.ts
sed -i '' 's/this\.style\.opacity = "0\.1";/this.style.opacity = "0.5";/g' src/esp-entity-chart.ts

# Make app component backgrounds transparent so the page background shows through
sed -i '' 's/background-color: var(--c-bg);/background-color: transparent;/g' src/css/app.ts

# 4. Deep Clean: Replace all hardcoded blue/teal brand colors in source files
echo "Replacing hardcoded blue colors in source files..."
find src -type f \( -name "*.ts" -o -name "*.css" \) -exec sed -i '' "s/#03a9f4/$BRAND_COLOR/g" {} +
find src -type f \( -name "*.ts" -o -name "*.css" \) -exec sed -i '' "s/rgb(3, 169, 244)/rgb(255, 145, 0)/g" {} +
find src -type f \( -name "*.ts" -o -name "*.css" \) -exec sed -i '' "s/#44739e/$BRAND_COLOR/g" {} +
find src -type f \( -name "*.ts" -o -name "*.css" \) -exec sed -i '' "s/#26a69a/$BRAND_COLOR/g" {} +

# 5. Inject global CSS and Font Link into main.ts
echo "Injecting NIX labs fonts and global styles into main.ts..."
BP_BRANDING_CSS="
/* =============================================
   NIX labs Global Style Injection
   Palette:
     Brand orange : #FF9100
     Page base    : #101828
     Card bg      : rgba(16, 24, 40, 0.8)
   Fonts:
     Body    : Inter
     Headings: Exo 2
   Note: Shadow DOM elements (.tab-container, .tab-header, .entity-row etc.)
   cannot be styled here — they are handled via source-level sed in step 3.
   ============================================= */

:root {
  --primary-color: $BRAND_COLOR !important;
  --c-primary: $BRAND_COLOR !important;
  --c-bg: $BG_BASE !important;
}

html {
  background-color: $BG_BASE !important;
}

body {
  font-family: 'Inter', sans-serif !important;
  background-color: transparent !important;
  min-height: 100vh;
}

/* Full-viewport background image with subtle blur */
body::before {
  content: '';
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-image: url('https://n33174.github.io/nixr_dev/static/bg.webp');
  background-size: cover;
  background-position: center;
  filter: blur(2px);
  z-index: -50;
  pointer-events: none;
}

/* Make structural wrappers transparent so the background shows through */
esp-app, main, .flex-grid-half, section, .col, .tab-container, .tab-header {
  background-color: transparent !important;
}

/* Floating glass card panels */
header, .col, esp-entity-table, esp-log {
  background: $BG_CARD !important;
  border: none !important;
  border-radius: 12px !important;
  box-shadow: 0 4px 24px rgba(0, 0, 0, 0.5) !important;
  backdrop-filter: blur(8px) !important;
  padding: 1rem !important;
}

/* Entity row subtle inset cards & OTA Form */
.entity-row, form.tab-container {
  background: rgba(0, 0, 0, 0.2) !important;
  border: none !important;
  border-radius: 10px !important;
  margin: 4px 0 !important;
  padding: 0.5rem 0.75rem !important;
  box-shadow: none !important;
  backdrop-filter: none !important;
}

/* Form field styling */
input[type=\"text\"],
input[type=\"date\"],
input[type=\"number\"],
input[type=\"file\"],
select,
textarea {
  background: rgba(255, 255, 255, 0.08) !important;
  border: 1px solid rgba(255, 145, 0, 0.4) !important;
  color: #ffffff !important;
  border-radius: 8px !important;
  padding: 0.5rem !important;
  outline: none !important;
  transition: border-color 0.2s ease, box-shadow 0.2s ease !important;
}

input[type=\"text\"]:focus,
input[type=\"date\"]:focus,
input[type=\"number\"]:focus,
input[type=\"file\"]:focus,
select:focus,
textarea:focus {
  border-color: #FF9100 !important;
  box-shadow: 0 0 8px rgba(255, 145, 0, 0.75) !important;
}

/* Slider accent colour */
input[type=\"range\"] {
  accent-color: #FF9100 !important;
}

/* Headings: Exo 2, NIX orange */
h1, header h1, .dashboard-title {
  font-family: 'Exo 2', sans-serif !important;
  font-weight: 700 !important;
  color: $BRAND_COLOR !important;
}

/* Tab headers: Exo 2, white */
.tab-header {
  font-family: 'Exo 2', sans-serif !important;
  font-weight: 700 !important;
  color: #ffffff !important;
}

/* Brand orange on primary action buttons only (not all icon buttons) */
ha-button, .call-service-button, button.primary {
  color: $BRAND_COLOR !important;
  font-weight: 500 !important;
}

button:hover, .btn:hover {
  background-color: rgba(255, 145, 0, 0.1) !important;
}

/* Entity first-child icon colour */
.entity-row > :nth-child(1) {
  color: $BRAND_COLOR !important;
}

/* Header icon and logo colour */
header iconify-icon, header #logo {
  color: $BRAND_COLOR !important;
}

/* Scrollbar styling */
::-webkit-scrollbar { width: 6px; height: 6px; }
::-webkit-scrollbar-track { background: $BG_BASE; }
::-webkit-scrollbar-thumb { background: rgba(255, 145, 0, 0.5); border-radius: 3px; }
::-webkit-scrollbar-thumb:hover { background: #FF9100; }
"

# Prepend the brand injector to main.ts — runs before Lit rendering begins
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

# 6. Install dependencies and build
echo "Installing dependencies..."
npm install

echo "Building branded webserver..."
npm run build

# 7. Extract results
echo "Locating build output..."

if [ -f "../../_static/v3/www.js" ]; then
    echo "Found www.js. Copying to static/..."
    cp "../../_static/v3/www.js" "$STATIC_OUT/www.js"
    echo "Successfully updated static/www.js"
else
    # Fallback: scan for Vite dist output
    JS_FILE=$(find dist -name "index-*.js" 2>/dev/null | head -n 1)
    if [ -n "$JS_FILE" ]; then
        cp "$JS_FILE" "$STATIC_OUT/www.js"
        echo "Successfully updated static/www.js from $JS_FILE"
    else
        echo "Error: Could not find build output www.js!"
        ls -R .
        exit 1
    fi
fi

echo "--- Branding complete! ---"
echo "Branded interface saved to: $STATIC_OUT/www.js"
echo "To repeat in the future, run: bash scripts/brand_webserver.sh"
