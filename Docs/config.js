module.exports = {
    siteTitle: 'Polly',
    siteUrl: 'https://polly2d.org',

    logo: {
        light: '/assets/images/logo-splash.svg',
        dark: '/assets/images/logo-splash-dark.svg',
        alt: 'Polly logo',
        href: '/',
    },

    srcDir: 'docs',
    outputDir: 'site',

    sidebar: {
        collapsible: true,
        defaultCollapsed: true,
    },

    theme: {
        name: 'sky',
        defaultMode: 'light',
        enableModeToggle: true,
        positionMode: 'top',
        codeHighlight: true,
        customCss: [
            '/assets/css/custom.css',
        ]
    },

    autoTitleFromH1: true,
    copyCode: true,

    plugins: {
        seo: {
            defaultDescription: 'Polly2D is 2D C++ game framework for minimalists.',
            openGraph: {
                defaultImage: '/assets/images/polly-preview.png',
            },
        },
        sitemap: {
            defaultChangefreq: 'weekly',
            defaultPriority: 0.8
        }
    },
    navigation: [
        {
            title: 'Introduction',
            path: '/',
            icon: 'home',
        },
        {
            title: 'GitHub',
            path: 'https://github.com/cdervis/Polly2D',
            icon: 'github',
            external: true
        },
        {
            title: 'Game',
            icon: 'dices',
            collapsible: true,
            children: [
                {
                    title: 'Assets',
                    icon: 'package',
                    path: 'game/assets'
                },
                {
                    title: 'Events',
                    icon: 'zap',
                    path: 'game/events'
                },
                {
                    title: 'Timing',
                    icon: 'timer',
                    path: 'game/timing'
                },
                {
                    title: 'Window',
                    icon: 'app-window-mac',
                    path: 'game/window'
                },
                {
                    title: 'Logging',
                    icon: 'scroll-text',
                    path: 'game/logging'
                },
                {
                    title: 'Display',
                    icon: 'monitor',
                    path: 'game/display'
                },
                {
                    title: 'Dear ImGui',
                    icon: 'layout-template',
                    path: 'game/dear-imgui'
                }
            ]
        },
        {
            title: 'Graphics',
            icon: 'gpu',
            collapsible: true,
            children: [
                {
                    title: 'Sprites',
                    path: 'graphics/sprites',
                    icon: 'image'
                },
                {
                    title: 'Samplers',
                    path: 'graphics/samplers',
                    icon: 'waypoints'
                },
                {
                    title: 'Text',
                    path: 'graphics/text',
                    icon: 'type'
                },
                {
                    title: 'Transformations',
                    path: 'graphics/transformations',
                    icon: 'move-3d'
                },
                {
                    title: 'Shading Language',
                    path: 'graphics/shading-language',
                    icon: 'code'
                },
                {
                    title: 'Custom Shaders',
                    path: 'graphics/custom-shaders',
                    icon: 'spray-can'
                },
                {
                    title: 'Polygons',
                    path: 'graphics/polygons',
                    icon: 'shapes'
                },
                {
                    title: 'Meshes',
                    path: 'graphics/meshes',
                    icon: 'pyramid'
                },
                {
                    title: 'Spine',
                    path: 'graphics/spine',
                    icon: 'person-standing'
                }
            ]
        },
        {
            title: 'Input',
            icon: 'cable',
            collapsible: true,
            children: [
                {
                    title: 'Mouse',
                    icon: 'mouse',
                    path: 'input/mouse'
                },
                {
                    title: 'Keyboard',
                    icon: 'keyboard',
                    path: 'input/keyboard'
                },
                {
                    title: 'Gamepad',
                    icon: 'gamepad-2',
                    path: 'input/gamepad'
                },
                {
                    title: 'Touch',
                    icon: 'pointer',
                    path: 'input/touch'
                },
                {
                    title: 'Text',
                    icon: 'text-cursor-input',
                    path: 'input/text'
                }
            ]
        },
        {
            title: 'Audio',
            icon: 'audio-lines',
            path: 'audio'
        },
        {
            title: 'C++',
            icon: 'braces',
            path: 'cpp'
        },
        {
            title: 'Package Management',
            icon: 'package-plus',
            path: 'package-management'
        },
        {
            title: 'Publishing',
            icon: 'message-square-share',
            path: 'publishing'
        },
        {
            title: 'Legal',
            icon: 'scale',
            collapsible: true,
            children: [
                {
                    title: 'License',
                    icon: 'copyright',
                    path: 'license'
                },
                {
                    title: 'Privacy',
                    icon: 'cookie',
                    path: 'privacy'
                },
                {
                    title: 'Imprint',
                    icon: 'signature',
                    path: 'imprint'
                }
            ]
        },
        {
            title: 'Contributing',
            path: 'contributing',
            icon: 'users-round'
        }
    ],

    footer: '© ' + new Date().getFullYear() + ' [Cem Dervis](https://dervis.de)',
    favicon: '/assets/images/favicon.ico',
};
