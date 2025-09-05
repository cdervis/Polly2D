module.exports = {
    siteTitle: 'Polly - 2D C++ Game Framework for Minimalists',
    siteUrl: 'https://polly2d.org',

    logo: {
        light: '/assets/images/logo-splash-light.webp',
        dark: '/assets/images/logo-splash-dark.webp',
        alt: 'Polly logo',
        href: '/',
    },

    srcDir: 'docs',
    outputDir: 'site',

    sidebar: {
        collapsible: true,
        defaultCollapsed: false,
    },

    theme: {
        name: 'sky',
        defaultMode: 'light',
        enableModeToggle: false,
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
            defaultDescription: 'Polly is a framework that makes it easy to get started with C++ 2D game development using a clean, modern API, for beginners and experts alike.',
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
            title: 'Home',
            path: '/',
            icon: 'home',
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
                    title: 'Canvas',
                    path: 'graphics/canvas',
                    icon: 'frame'
                },
                {
                    title: 'Particles',
                    icon: 'sparkles',
                    path: 'graphics/particles',
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
            title: 'Math',
            icon: 'radical',
            collapsible: true,
            children: [
                {
                    title: 'Basics',
                    icon: 'plus',
                    path: 'math/basics'
                },
                {
                    title: 'Algorithms',
                    icon: 'route',
                    path: 'math/algorithms',
                },
                {
                    title: 'Random',
                    icon: 'badge-question-mark',
                    path: 'math/random'
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
            title: 'Packages',
            icon: 'package-plus',
            path: 'package-management'
        },
        {
            title: 'Publishing',
            icon: 'rss',
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
            path: '/contributing',
            icon: 'users-round'
        },
        {
            title: 'GitHub',
            path: 'https://github.com/cdervis/Polly2D',
            icon: 'github',
            external: true
        }
    ],

    madeWithDocmd: 'Made with',

    footer: '© ' + new Date().getFullYear() + ' [Cem Dervis](https://dervis.de)',
    favicon: '/assets/images/favicon.ico',
};
